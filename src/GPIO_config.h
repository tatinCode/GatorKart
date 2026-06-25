/**************************************************************
 * Class:: CSC-615-01 Spring 2026
 * Names:: Justine Tenorio,Alejandro Huang, 
 *         Kerry Yu, Wilmaire Mejia, Samantha Chombo-Rodriguez
 * Github-Name:: tatinCode
 * Project:: Term Project
 *
 * File:: GPIO_config.h
 *
 * Description:: 
 *  This file contains the GPIO configuration for the motors
 *  and the RGB sensor and stuff.
 **************************************************************/

#define MOTOR_PIN_1 0

//#define LEFT_LINE_PIN 23
//#define CENTER_LINE_PIN 24
//#define RIGHT_LINE_PIN 25

#define LEFT_LINE_PIN 25
#define CENTER_LINE_PIN 24
#define RIGHT_LINE_PIN 23

#define RGB_LED_PIN 15

#define LIDAR_FRONT_STOP 0.22f

#define SENSOR_POLL_US 20000

#define STATUS_POLL_US 30000

#define LINE_DETECTED 0
#define LINE_NOT_DETECTED 1
