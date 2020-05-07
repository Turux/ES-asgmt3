//***********************************************************************
//
//  Embedded Software
//  B31DG
//
//  Assignment 3
//
//  Author: Lorenzo Stilo
//  Date: Mar 2016
//
//************************************************************************
//
//  main.cpp
//  Version: 1.5 (Milestone 1)
//
//  Device: mbed LPC1768
//  WattBob I
//
//  Requirements: MCP23017.h, MCP23017.cpp, WattBob_TextLCD.h WattBob_TextLCD.cpp
//                car.h, car.cpp, controller.h, controller.cpp, message.h, pinout.h
//
//
//************************************************************************
//
//  Initialize an object of type Controller
//  Once the object is fully cunstructed the programm will run 11 main Threads
//
//************************************************************************

/* Class includes */
#include "controller.h"

int main() 
{
    /* Declare an object of Controller Class */
    Controller CarController;
    
    /* Waits forever */
    while (1);
}
