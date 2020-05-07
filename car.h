//************************************************************************
//
//  car.h
//
//  Requirements: rtos.h
//
//  Defines a Car Class that proviedes a simple simulator of the 
//  behaviour of a car vehicle.
//
//  Class members:
//          -accelerator    (uint8_t)
//          -brake          (uint8_t)
//          -speed          (uint8_t)
//          -distance       (uint16_t)
//          -engine         (bool)
//          -side_light     (bool)
//          -left_indicator (bool)
//          -right_indicator(bool)
//
//  Methods:  
//          -This class provides standard accessors to every member of the class
//          -Updates Car status in accords to Engine status
//          -Updates speed and distance in accords to acceleration value
//
//
//  Threads: 
//          This class provides a thread updating speed and distance 
//          according to accelerator and brake value.
//          Repetition rate 20Hz
//
//
//************************************************************************
#ifndef __CAR_H__
#define __CAR_H__

/* RTOS Includes */
#include "rtos.h"

class Car
{
    public:
        /* Default Constructor */
        Car();
        
        /* Static Callback to thread */
        static void threadStarter(void const *p);
        
        /* Standard Accessors */
        char getAcc();
        void writeAcc(char Acc);        
        char getBrake();
        void writeBrake(char Brake);                
        bool getSide();
        void writeSide(bool Side);        
        bool getLeft();
        void writeLeft(bool Left);        
        bool getRight();
        void writeRight(bool Right);
        int  getDistance();
        char getSpeed();
        bool IsItOn();
        
        /* Thread worker */
        void updateSpeed();
        
        /* Methods to update car status in accords to the engine value */
        void TurnOn();
        void TurnOff();
        
        /* Shared semaphore */
        Semaphore Pedals;
    
    private:
        void writeSpeed(char Speed);
        bool getEng();
        void writeEng(bool Eng);
    
    protected:
        /* Members */
        char accelerator;
        char brake;
        char speed;
        unsigned short distance;
        bool engine;
        bool side_light;
        bool left_indicator;
        bool right_indicator;
        
        /* Threads */
        Thread _thread;
};

#endif