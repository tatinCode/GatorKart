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
#include "rgb_controller.h"
#include "RGB_Sensor.h"
#include "GPIO_config.h"

#include <stdio.h>
#include <stddef.h>

int rgb_init(){
    if(RGB_BusInit("/dev/i2c-1") != 0){
        printf("Failed to initialize RGB sensor\n");
        return -1;
    }

    if(TCS34725_Init(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X) != 0){
        RGB_BusCleanup();
        printf("Failed to initialize TCS34725 sensor\n");
        return -1;
    }
//    TCS34725_SetLight(40);

    return 0;
}

int rgb_read(ColorResult *color){
    RGB rgb;

    if(color == NULL){
        return 0;
    }

    ReadAvgRGB(&rgb, 5, 30);

    return AnalyzeColor(&rgb, color);
}

void rgb_cleanup(){
    //TCS34725_SetLight(0);
    RGB_BusCleanup();
}




