//************************************************************************
//
//  message.h
//
//  Defines an object of type 'message'
//
//  Members:
//          -speed      (uint8_t)
//          -accelerator(uint8_t)
//          -brake      (uint8_t)
//
//************************************************************************
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

typedef struct {
  char  speed;
  char  accelerator;
  char  brake;
} message;

#endif