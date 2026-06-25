/**************************************************************
* Class:: CSC-615-01 Spring 2026
* Names:: Justine Tenorio, Alejandro Huang
* Github-Name:: tatinCode 
* Project:: Term Project
*
* File:: motor_driver.h
*
* Description:: This header file defines the interface for the 
* motor driver subsystem, which includes the initialization, 
* update, and cleanup functions. It also defines the possible 
* states of the motor driver.
* 
*
**************************************************************/
#pragma once

// These states give the rest of the robot a simple way to react to
// line position without working directly with raw line sensor values.
typedef enum {
    LINE_STATE_CENTERED,
    LINE_STATE_HARD_LEFT,
    LINE_STATE_HARD_RIGHT,
    LINE_STATE_LIGHT_LEFT,
    LINE_STATE_LIGHT_RIGHT,
    LINE_STATE_INTERSECTION,
    LINE_STATE_LOST_LINE,
    LINE_STATE_UNDEFINED
} LineState;

// Starts the line sensor subsystem so background polling can support
// line-following decisions during robot operation.
int line_sensor_init(void);

// Updates the program line-following decision using the latest sensor readings.
LineState line_sensor_update(void);

// Stops line sensor polling and releases GPIO resources cleanly.
void line_sensor_cleanup(void);

