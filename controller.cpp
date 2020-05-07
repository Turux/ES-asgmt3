//************************************************************************
//
//  controller.cpp
//
//  Controller Class
//
//************************************************************************

/* Header includes */
#include "controller.h"

/* Pinout includes */
#include "pinout.h"

/*  LCD Initialization */
//  @brief  Initialize LCD and prints layout
//
//  N.B.: Uses semaphore
void Controller::LCDInit()
{
    // Wait for the LCD semaphore
    LCDs.wait();
    // Initialise 16-bit I/O chip
    par_port = new MCP23017(p9, p10, 0x40); 
    
    // Initialise 2x26 char display
    lcd = new WattBob_TextLCD(par_port); 
    
    // Turn LCD backlight ON
    par_port->write_bit(1,BL_BIT); 
    
    // Clear display
    lcd->cls();
    
    // Display Initial Layout
    lcd->locate(0,0);
    lcd->printf("    mph");
    lcd->locate(1,7);
    lcd->printf(" m");
    
    //Relese the LCD semaphore
    LCDs.release();
}

/*  Serial Initialization */
//  @brief  Initialize Serial and prints headers
//
//  N.B.: Uses semaphore
void Controller::SerialInit()
{   
    Serials.wait();
     
    // Set Baud Rate
    serial.baud(115200);
    
    // Print heading for CSV
    serial.printf("speed, acceleretor, brake\r\n");
    
    Serials.release();

}

/*  Flashes an indicator */
//  @brief  Flashes a LED at 1Hz
//
//  N.B.: Thread Worker
void Controller::Flash1Hz()
{
    while (1)
    {
        Thread::signal_wait(1);
        right_led = Simulator.getRight();
        left_led = Simulator.getLeft();
        wait_ms(400);
        right_led = 0;
        left_led = 0;
        Thread::wait(750);
    }
}

/*  Flashes two indicators */
//  @brief  Flashes two LEDs at 2Hz
//
//  N.B.: Thread Worker
void Controller::HazardMode()
{
    while(1)
    {
        Thread::signal_wait(1);
        right_led = 1;
        left_led = 1;
        wait_ms(200);
        right_led = 0;
        left_led = 0;
        Thread::wait(400);
    }
}

/*  Default Constructor */
//  @brief  Init Semaphore, Threads, Serial, LCD, Car Simulator
//          Set average speed and warning led to 0
Controller::Controller()
:   serial(USBTX, USBRX),
    Speeds(4),
    Mails(1),
    Serials(1),
    LCDs(1),
    updateCommandsTh(&Controller::commandsStarter, this, osPriorityNormal,1024),
    updateEngineTh(&Controller::engineStarter, this, osPriorityNormal,1024),
    updateSpeedTh(&Controller::speedStarter, this, osPriorityNormal,1024),
    driveServoTh(&Controller::servoStarter, this, osPriorityNormal,1024),
    updateWarningTh(&Controller::warningStarter, this, osPriorityNormal,1024),
    driveOdoTh(&Controller::odoStarter, this, osPriorityNormal,1024),
    sendMailTh(&Controller::mailStarter, this, osPriorityNormal,1024),
    sendSerialTh(&Controller::serialStarter, this, osPriorityNormal,1024),
    updateSidelightTh(&Controller::sideStarter, this, osPriorityNormal,1024),
    driveIndicatorsTh(&Controller::indicatorStarter, this, osPriorityNormal,1024),
    flashTh(&Controller::flashStarter, this, osPriorityNormal,1024),
    hazardTh(&Controller::hazardStarter, this, osPriorityNormal,1024)
{
    speed_warning = 0;
    speed_average = 0;
    LCDInit();
    SerialInit();
}

/*  Updates speed history */
//  @brief  pushes a raw valued of speed in the history queue
void Controller::updateHistory()
{
    char speed = Simulator.getSpeed();
    speed_history.push(speed);
}

/*  Updates acceleration  */
//  @pram   pin     analog pin
//  @brief  reads acceleration value from analog pin
void Controller::updateAcceleration(AnalogIn pin)
{
    float reading = (pin.read() * 255);
    char acceleration = reading;
    Simulator.writeAcc(acceleration);
}

/*  Updates brake  */
//  @pram   pin     analog pin
//  @brief  reads brake value from analog pin
void Controller::updateBrake(AnalogIn pin)
{
    float reading = (pin.read() * 255);
    char brake = reading;
    //serial.printf("B = %i", brake);
    //serial.printf("\r\n");
    Simulator.writeBrake(brake);
}

/*  Calculates average */
//  @return     average speed
//  @brief      average 3 raw value from speed_history
//              also updates speed_warning
char Controller::getAverage()
{
    if (speed_history.size() < 4)
        {
            return speed_average;    
        }
    else
        {
            char one = speed_history.front();
            speed_history.pop();
            char two = speed_history.front();
            speed_history.pop();
            char three = speed_history.front();
            speed_history.pop();
            speed_average = (one + two + three)/3;
            if (speed_average > 70)
                speed_warning = 1;
            else
                speed_warning = 0;
            return speed_average;
        }
}

/*  Updates Indicators */
//  @brief  updates Indicators valued reading from digital inputs
void Controller::updateIndicators(DigitalIn left, DigitalIn right)
{
    Simulator.writeLeft(left);
    Simulator.writeRight(right);
}

/*  Flashes Indicators */
//  @brief  Trigger threads in accords to Indicators values
void Controller::flashIndicators()
{

    if(Simulator.getLeft() && Simulator.getRight())
        hazardTh.signal_set(1);
    else if (Simulator.getLeft())
        flashTh.signal_set(1);
    else if (Simulator.getRight())
        flashTh.signal_set(1);
    else
        {
            left_led = 0;
            right_led = 0;
        }
}
/*  Updates Commands */
//  @brief  updates acceleration, brake
//  @rate   10Hz
//
//  N.B.:   Uses semaphore
//  N.B.:   Thread worker
void Controller::updateCommands()
{
    while(1)
    {
        Simulator.Pedals.wait();
        updateAcceleration(accelerator_pedal);
        updateBrake(brake_pedal);
        Simulator.Pedals.release();
        Thread::wait(100);
    }
}

/*  Updates Engine */
//  @brief  updates engine value and turn the car simulator
//          on or off accordingly
//  @rate   2Hz
//
//  N.B.:   Thread worker
void Controller::updateEngine()
{
    while(1)
    {
        if (engine_sw)
        {
            Simulator.TurnOn();
            engine_led = 1;
        }
        else
        {
            Simulator.TurnOff();
            engine_led = 0;
        }
        Thread::wait(500);
    }
}

/*  Updates Speed */
//  @brief  updates speed history queue, calculates average
//  @rate   5Hz
//
//  N.B.:   Uses semaphore
//  N.B.:   Thread worker
void Controller::updateSpeed()
{
    while(1)
    {
        Speeds.wait();
        updateHistory();
        speed_average = getAverage();
        Speeds.release();
        Thread::wait(200);
    }
}

/*  Drive Servo */
//  @brief  updates servo position in accords to average speed
//  @rate   1Hz
//
//  N.B.:   Thread worker
void Controller::driveServo()
{
    while(1)
    {
        float position = speed_average / 255.0;
        motor = position;
        Thread::wait(1000);
    }
}

/*  Updates Warning */
//  @brief  updates warning LED for exceeding speed limit
//  @rate   0.5Hz
//
//  N.B.:   Thread worker
void Controller::updateWarning()
{
    while(1)
    {
        warning = speed_warning;
        Thread::wait(2000);
    }
}

/*  Drive Odometer */
//  @brief  write distance and average speed on the LCD Odometer
//  @rate   2Hz
//
//  N.B.:   Uses semaphore
//  N.B.:   Thread worker
void Controller::driveOdo()
{
    while(1)
    {
        Speeds.wait();
        int distance = Simulator.getDistance();
        char speed = speed_average;
        Speeds.release();
        LCDs.wait();
        lcd->locate(1,0);
        lcd->printf("%06i",distance);
        lcd->locate(0,0);
        lcd->printf("%03i",speed);
        if(Simulator.IsItOn())
        {
            lcd->locate(0,13);
            lcd->printf("   ");
        }
        else
        {
            lcd->locate(0,13);
            lcd->printf("(P)");
        }
        LCDs.release();
        Thread::wait(500);
        
    }
}

/*  Updates the send_queue */
//  @brief  push a new 'message' in the send_queue 
//  @rate   0.2Hz
//
//  N.B.:   Uses semaphore
//  N.B.:   Thread worker
void Controller::sendMail()
{
    while(1)
    {
        Mails.wait();
        message *mail = send_queue.alloc();
        Speeds.wait();
        mail->speed = speed_average;
        Speeds.release();
        mail->accelerator = Simulator.getAcc();
        mail->brake = Simulator.getBrake();
        send_queue.put(mail);
        Mails.release();
        Thread::wait(5000);
        
    }
}

/*  Send a Message over serial */
//  @brief  pop the send_queue and send a 'message' over serial
//  @rate   0.05Hz
//
//  N.B.:   Uses semaphore
//  N.B.:   Thread worker
void Controller::sendSerial()
{
    while(1)
    {
        Mails.wait();
        osEvent evt = send_queue.get();
        if (evt.status == osEventMail) 
        {
            message *mail = (message*)evt.value.p;
            serial.printf("%03i,", mail->speed);
            serial.printf("%03i,", (mail->accelerator));
            serial.printf("%03i", (mail->brake));
            serial.printf("\r\n");
            send_queue.free(mail);
        }
        Thread::wait(15000);
        Mails.release();
    }
}

/*  Updates Sidelight */
//  @brief  updates sidelight and flashes an LED accordingly 
//  @rate   1Hz
//
//  N.B.:   Thread worker
void Controller::updateSidelight()
{
    while(1)
    {
        Simulator.writeSide(sidelight_sw);
        sidelight_led = Simulator.getSide();
        Thread::wait(1000);
    } 
}

/*  Drive Indicators */
//  @brief  updates indicators and flashes an LEDs accordingly 
//  @rate   0.5Hz
//
//  N.B.:   Thread worker
void Controller::driveIndicators()
{
    while(1)
    {
        updateIndicators(left_sw, right_sw);
        flashIndicators();
        Thread::wait(2000);
    }
}

/* Static callback to Thread worker */
void Controller::commandsStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->updateCommands();
}

/* Static callback to Thread worker */
void Controller::engineStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->updateEngine();
}

/* Static callback to Thread worker */
void Controller::speedStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->updateSpeed();
}

/* Static callback to Thread worker */
void Controller::servoStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->driveServo();
}

/* Static callback to Thread worker */
void Controller::warningStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->updateWarning();
}

/* Static callback to Thread worker */
void Controller::odoStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->driveOdo();
}

/* Static callback to Thread worker */
void Controller::mailStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->sendMail();
}

/* Static callback to Thread worker */
void Controller::serialStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->sendSerial();
}

/* Static callback to Thread worker */
void Controller::sideStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->updateSidelight();
}

/* Static callback to Thread worker */
void Controller::indicatorStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->driveIndicators();
}

/* Static callback to Thread worker */
void Controller::flashStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->Flash1Hz();
}

/* Static callback to Thread worker */
void Controller::hazardStarter(void const *p)
{
    Controller *instance = (Controller*)p;
    instance->HazardMode();
}