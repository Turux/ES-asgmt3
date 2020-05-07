//************************************************************************
//
//  pinout.h
//
//  Defines Mbed pins used in Assignment 3
//
//************************************************************************
#ifndef __PINOUT_H__
#define __PINOUT_H__

/* Mbed includes */
#include "mbed.h"

/* Servo includes */
#include "Servo.h"

/* Digital Outputs */
DigitalOut engine_led(LED1);
DigitalOut sidelight_led(LED2);
DigitalOut left_led(LED3);
DigitalOut right_led(LED4);
DigitalOut warning(p11);

/* Digital Inputs */
DigitalIn engine_sw(p5);
DigitalIn sidelight_sw(p6);
DigitalIn left_sw(p7);
DigitalIn right_sw(p8);

/* Analog Inputs */
AnalogIn  accelerator_pedal(p17);
AnalogIn  brake_pedal(p16);  

/* Servo motors */
Servo motor(p21);

#endif