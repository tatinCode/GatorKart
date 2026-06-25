/**************************************************************
* Class:: CSC-615-01 Spring 2026
* Names:: Justine Tenorio,Alejandro Huang, 
*         Kerry Yu, Wilmaire Mejia, Samantha Chombo-Rodriguez
* Github-Name:: tatinCode
* Project:: Term Project
*
* File:: rgb_controller.h
*
* Description:: Defines the interface and data structure for 
*  color matching and analysis based off values given by the
*  TCS34725 sensor.
*
**************************************************************/
#pragma once

#include "color_match.h"

#include <stdint.h>

int rgb_init();
int rgb_read(ColorResult *color);
void rgb_cleanup();

