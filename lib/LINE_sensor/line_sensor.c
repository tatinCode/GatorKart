/**************************************************************
* Class:: CSC-615-01 Spring 2026
* Names:: Justine Tenorio, Alejandro Huang
* Github-Name:: tatinCode
* Project:: Term Project
*
* File:: line_sensor.c
*
* Description:: This file implements the line sensor subsystem,
* which includes the intialization, update, and cleanup functions.
* It also includes the background thread for polling the line
* sensor and helper functions.
* 
*
**************************************************************/

#include "line_sensor.h"

#include <stdio.h>
#include <pigpio.h>

#include "../../src/config.h"
#include "../../src/GPIO_config.h"

// Grouping the thread handle, pin assignments, and shared readings in one
// place keeps the line sensor subsystem synchronized and easier to manage as a unit.
typedef struct {
    int left_pin;
    int center_pin;
    int right_pin;
    int ready;
} LineSensorSystem;

// Keeping the line sensor state and helper functions private to this file avoids
// exposing line-following internals outside the sensor module.
static LineSensorSystem line_system;

// The program needs continuous sensor updates before it can make line-following
// decisions, so initialization sets up the shared runtime state and starts the
// background sensor threads.
int line_sensor_init(void) {
    LineSensorSystem *system = &line_system;

    system->ready = 0;

    system->left_pin = LEFT_LINE_PIN;
    system->center_pin = CENTER_LINE_PIN;
    system->right_pin = RIGHT_LINE_PIN;

//    gpioCfgSetInternals(PI_CFG_NOSIGHANDLER);

//    if (gpioInitialise() < 0) {
//        fprintf(stderr, "Failed to initialize pigpio.\n");
//        return -1;
//    }

    gpioSetMode(system->left_pin, PI_INPUT);
    gpioSetMode(system->center_pin, PI_INPUT);
    gpioSetMode(system->right_pin, PI_INPUT);

    gpioSetPullUpDown(system->left_pin, PI_PUD_UP);
    gpioSetPullUpDown(system->center_pin, PI_PUD_UP);
    gpioSetPullUpDown(system->right_pin, PI_PUD_UP);

    system->ready = 1;
// $(BUTTON_SRCS)    printf("3-sensor line follower init\n");

    return 0;
}
// The program needs to regularly turn the latest sensor readings into driving
// decisions, so this update step returns the current line state for the
// controller to act on.
LineState line_sensor_update(void) {
    int left;
    int center;
    int right;
    LineSensorSystem *system = &line_system;

    if (!system->ready) {
        return LINE_STATE_LOST_LINE;
    }

    left = gpioRead(system->left_pin);
    center = gpioRead(system->center_pin);
    right = gpioRead(system->right_pin);

    printf("RAW left=%d center=%d right=%d\n", left, center, right);

    if (!left && center && !right) {    // 1
        //printf("State: CENTERED\n");
        return LINE_STATE_CENTERED;
    }
    if (left && center && !right) {     // 2
        //printf("State: DRIFTING LEFT\n");
        return LINE_STATE_HARD_LEFT;
    }
    if (!left && center && right) {     // 3
        //printf("State: DRIFTING RIGHT\n");
        return LINE_STATE_HARD_RIGHT;
    }
    if (!left && !center && right) {    // 6
        //printf("State: LEFT & CENTER LOST\n");
        return LINE_STATE_LIGHT_LEFT;
    }
    if (left && !center && !right) {    // 7
        //printf("State: RIGHT & CENTER LOST\n");
        return LINE_STATE_LIGHT_RIGHT;
    }
    if (!left && !center && !right) {   // 4
        //printf("State: LOST LINE\n");
        return LINE_STATE_LOST_LINE;
    }
    if (left && !center && right) {     // 8
        //printf("State: NOT CENTERED\n");
        return LINE_STATE_INTERSECTION;
    }

    return LINE_STATE_UNDEFINED;
}

// The sensor thread and GPIO library must be shut down cleanly so the robot
// does not leave background work or hardware resources running after stop.
void line_sensor_cleanup(void) {
    LineSensorSystem *system = &line_system;

    if (!system->ready) {
        return;
    }

    system->ready = 0;
}

