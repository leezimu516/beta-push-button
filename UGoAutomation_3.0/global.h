#ifndef GLOBAL_H
#define GLOBAL_H

#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "Arduino.h"
#include <stdio.h>
#include "usb_comm.h"

    
#define USB_COMMUNICATION

#ifdef __cplusplus 
extern "C" {
#endif
  #include "logger.h"
  #include "mediator.h"
#ifdef __cplusplus 
}
#endif

#define NUMBER_OF_MACHINES 1

#define FIRMWARE_VERSION_MAJOR 0
#define FIRMWARE_VERSION_MINOR 0
#define FIRMWARE_REVISION      1

#define MACHINE_STATE_IDLE 0
#define MACHINE_STATE_BLENDING 1
#define MACHINE_STATE_CLEANING 2
#define MACHINE_STATE_INITIALIZING 3
#define MACHINE_STATE_STEPPING 4

#define MACHINE_CYCLE_TYPE_AUTO 0
#define MACHINE_CYCLE_TYPE_STEP 1

#define ON HIGH
#define OFF LOW


//Distance Calibration Measurments

// 12" Actuator
#ifdef TWELVE_INCH_ACTUATOR
  #define TOP_POSITION 145
  #define TOP_OF_CUP 210//310
  #define TOP_OF_SMOOTHIE 250//340 
  #define BOTTOM_OF_CUP 340//405
  #define BOTTOM_OF_CLEANING 405
  #define CLEANING_LEVEL 358 
#else
//6" Actuator
  //#define TOP_POSITION 330
  //#define TOP_OF_CUP 430
  //#define TOP_OF_SMOOTHIE 460
  //#define BOTTOM_OF_CUP 625
  //#define BOTTOM_OF_CLEANING 650
  //#define CLEANING_LEVEL 400
  
//4" Actuator  
  #define TOP_POSITION 360 //320
  #define TOP_OF_CUP 450 // 380
  #define TOP_OF_SMOOTHIE 455 // 410
  #define BOTTOM_OF_CUP 565//595//585
  #define BOTTOM_OF_CLEANING 660 
  #define CLEANING_LEVEL 573
#endif // actuator length

#endif
