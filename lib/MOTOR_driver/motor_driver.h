/**************************************************************
* Class:: CSC-615-01 Spring 2026
* Names:: Samantha Chombo-Rodriguez
* Github-Name:: tatinCode
* Project:: Term Project
*
* File:: motor_driver.h
*
* Description:: This header file defines the interfaces for the
* motor driver, including the motor control functions and the
* structures for motor channels and commands.
*
**************************************************************/

#pragma once

#include "MOTOR_driver/PCA9685.h"

typedef struct  {
	UBYTE pwm;
	UBYTE in1;
	UBYTE in2;
} MotorChannels;

typedef enum {
    MOTOR_FORWARD,
    MOTOR_BACKWARD,
    MOTOR_LEFT,
    MOTOR_RIGHT,
    MOTOR_STOP, 
    MOTOR_PIVOT_LEFT, 
    MOTOR_PIVOT_RIGHT,
    MOTOR_ADJUST_LEFT,
    MOTOR_ADJUST_RIGHT,
    MOTOR_SHIFT_LEFT,
    MOTOR_SHIFT_RIGHT
} MotorCommands;

void motor_init();
void motor_update(MotorCommands);
void motor_stop(I2CDevice *hataddy, MotorChannels motor);
void all_stop();
void motor_cleanup ();
