//************************************************************************
//
//  car.cpp
//
//  Car Class
//
//************************************************************************

/* Header includes */
#include "car.h"

/*  Default Constructor */
//  @brief  Initialize Semaphore, Threads and puts the 
//          Car object in Off Mode
Car::Car()
: Pedals(2),
_thread(&Car::threadStarter, this, osPriorityNormal,1024)
{
    accelerator = 0;
    brake = 0;
    speed = 0;
    engine = 0;
    side_light = 0;
    left_indicator = 0;
    right_indicator = 0;
    distance = 0;
}
/*  Standard Accessor */
char Car::getAcc()
{
    return accelerator;
}

/*  Standard Accessor */
char Car::getBrake()
{
   return brake;        
}

/*  Standard Accessor */
char Car::getSpeed()
{
     return speed;
}

/*  Standard Accessor */
bool Car::getEng()
{
    return engine;
}

/*  Standard Accessor */
bool Car::getSide()
{
    return side_light;
}

/*  Standard Accessor */
bool Car::getLeft()
{
    return left_indicator;
}

/*  Standard Accessor */
bool Car::getRight()
{
    return right_indicator;
}

/*  Standard Accessor */
int Car::getDistance()
{
    return distance;
}

/*  Standard Accessor */
//  @param      Acc     New acceleration value
//  
//  @brief      updates acceleration in accords to 
//              car status
void Car::writeAcc(char Acc)
{
    if (engine)
        accelerator = Acc;
    else
        accelerator = 0;
}

/*  Standard Accessor */
//  @param      Brake     New brake value
//  
//  @brief      updates brake in accords to 
//              car status
void Car::writeBrake(char Brake)
{
    if (engine)
        brake = Brake;
    else
        brake = 0;
}

/*  Standard Accessor */
//  @param  Eng     engine value
void Car::writeEng(bool Eng)
{
    engine = Eng;
}

/*  Standard Accessor */
//  @param  Side    sidelight value
void Car::writeSide(bool Side)
{
    if (engine)
        side_light = Side;
    else
        side_light = 0;
}

/*  Standard Accessor */
//  @param  Left    indicator value
void Car::writeLeft(bool Left)
{
    if (engine)
        left_indicator = Left;
    else
        left_indicator = 0;
}

/*  Standard Accessor */
//  @param  Right    indicator value
void Car::writeRight(bool Right)
{
    if (engine)
        right_indicator = Right;
    else
        right_indicator = 0;
}

/*  Standard Accessor */
//  @param  Speed    speed value
//  @brief  PRIVATE METHOD
void Car::writeSpeed(char Speed)
{
    if (engine)
        speed = Speed;
    else
        speed = 0;
}

/*  Standard Accessor */
bool Car::IsItOn()
{
    return engine;
}

/*  Thread worker */
//  @rate   20Hz
//  @brief  updates speed in accords to acceleration and brake value
//
//  N.B.:Time delta consistent with repetition rate
//  N.B.:Uses Semaphore
void Car::updateSpeed()
{
    while(1)
    {
        Pedals.wait();
        if (engine)
        {
            unsigned short test = speed + ((accelerator - brake) * 0.05);
            if (test < 255)
            speed = test;
            else 
            speed = 255;
        }
        else
            speed = 0;
        distance = distance + (speed * 0.05);
        Pedals.release();
        Thread::wait(2000);
    }
}

/*  Turn the Car On */
//  @brief      sets the Car in ON mode
void Car::TurnOn()
{
    engine = 1;
}

/*  Turn the Car On */
//  @brief      sets the Car in OFF mode
void Car::TurnOff()
{
    engine = 0;
    speed = 0;
    accelerator = 0;
    brake = 0;
    speed = 0;
    engine = 0;
    side_light = 0;
    left_indicator = 0;
    right_indicator = 0;
}

/*  Thread static callback */
//  @brief      Calls updateSpeed method
void Car::threadStarter(void const *p) 
{
  Car *instance = (Car*)p;
  instance->updateSpeed();
}