//************************************************************************
//
//  controller.h
//
//  Requirements: rtos.h, mbed.h, message.h, car.h, Servo.h, MCP23017.h,
//                WattBob_TextLCD.h
//
//  Hardware Requirements:
//          -Serial USB port
//          -WattBob LCD
//
//  Defines a Controller Class that controls a Car Object
//  
//
//  Class members:
//          -Simulator      (Car)
//          -speed_history  (queue<uint8_t>)
//          -speed_average  (uint8_t)
//          -speed_warning  (bool)
//          -send_queue     (message)*
//
//  Methods:  
//          -This class provides standard accessors to every member of the class
//          -updateCommands         updates acceleration and brake
//          -updateEngine           updates engine status
//          -updateSpeed            updates speed by avaraging
//          -driveServo             updates servo position in accords to speed
//          -updateWarning          updates a warning if speed goes over 70mph         
//          -driveOdo               updates Odometer
//          -sendMail               build a 'message' and pushes into a Mail queue
//          -sendSerial             send a 'message' over serial 
//          -updateSidelight        updates sidelight
//          -driveIndicators        updates indicators
//
//  Threads: 
//          -updateCommandsTh       calls 'updateCommands'  rate = 10Hz
//          -updateEngineTh         calls 'updateEngine'    rate = 2Hz
//          -updateSpeedTh          calls 'updateSpeed'     rate = 5Hz
//          -driveServoTh           calls 'driveServo'      rate = 1Hz
//          -updateWarningTh        calls 'updateWarning'   rate = 0.5Hz
//          -driveOdoTh             calls 'driveOdo'        rate = 2Hz
//          -sendMailTh             calls 'sendMail'        rate = 0.2Hz
//          -sendSerialTh           calls 'sendSerial'      rate = 0.05Hz
//          -updateSidelightTh      calls 'updateSidelight' rate = 1Hz
//          -driveIndicatorsTh      calls 'driveIndicators' rate = 0.5Hz
//          -flashTh                Flashes a LED in accords to indicators
//                                                          rate = 1Hz
//          -hazardTh               Flashed two LEDs in accords to indicatos
//                                                          rate = 2Hz
//
//
//  * Mail queue
//
//************************************************************************
#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

/* Inheritance includes */
#include "car.h"
#include "message.h"

/* Mbed & RTOS includes */
#include "mbed.h"
#include "rtos.h"

/* Hardware includes */
#include "MCP23017.h"
#include "WattBob_TextLCD.h"
#include "Servo.h"

/* Standard includes */
#include <queue>

using namespace std;

class Controller
{
    public:
        /* Default Constructor */
        Controller();
        
        /* Static callbacks to Threads */
        static void commandsStarter(void const *p);
        static void engineStarter(void const *p);
        static void speedStarter(void const *p);
        static void servoStarter(void const *p);
        static void warningStarter(void const *p);
        static void odoStarter(void const *p);
        static void mailStarter(void const *p);
        static void serialStarter(void const *p);
        static void sideStarter(void const *p);
        static void indicatorStarter(void const *p);
        static void flashStarter(void const *p);
        static void hazardStarter(void const *p);
        
        /* Threads workers */        
        void updateCommands();
        void updateEngine();
        void updateSpeed();
        void driveServo();
        void updateWarning();
        void driveOdo();
        void sendMail();
        void sendSerial();
        void updateSidelight();
        void driveIndicators();
        
    private:
        void updateHistory();
        void updateAcceleration(AnalogIn pin);
        void updateBrake(AnalogIn pin);
        void updateIndicators(DigitalIn left, DigitalIn right);
        char getAverage();
        void flashIndicators();
        
        /* Hardware Init */
        void LCDInit();
        void SerialInit();
        
        /* Indicators threads workers */
        void Flash1Hz();
        void HazardMode();
    
    protected:
        /* Members */
        Car Simulator;
        queue<char> speed_history;
        char speed_average;
        bool speed_warning;
        WattBob_TextLCD *lcd;
        MCP23017 *par_port;
        Serial serial;
        Mail<message, 100> send_queue;
        
        /*Semaphores */
        Semaphore Speeds;
        Semaphore Mails;
        Semaphore Serials;
        Semaphore LCDs;
        
        /* Threads */
        Thread updateCommandsTh;
        Thread updateEngineTh;
        Thread updateSpeedTh;
        Thread driveServoTh;
        Thread updateWarningTh;
        Thread driveOdoTh;
        Thread sendMailTh;
        Thread sendSerialTh;
        Thread updateSidelightTh;
        Thread driveIndicatorsTh;
        Thread flashTh;
        Thread hazardTh;
};

#endif