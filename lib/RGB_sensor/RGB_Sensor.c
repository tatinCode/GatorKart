/**************************************************************
* Class:: CSC-615-01 Spring 2026
* Names:: Justine Tenorio,Alejandro Huang, 
*         Kerry Yu, Wilmaire Mejia, Samantha Chombo-Rodriguez
* Github-Name:: tatinCode
* Project:: Term Project
*
* File:: RGB_Sensor.c
*
* Description:: Ulitlizes the BCM2835 libary to control 
*  the TCS34725 RGB Sensor. Provides functions for 
*  initializing the sensor, reading RGB values, and
*  controlling the light. 
* 
*
**************************************************************/
#include "RGB_Sensor.h"
#include "GPIO_config.h"

#include <stdio.h>
#include <unistd.h>
#include <pigpio.h>

static I2CDevice rgb_device;

int RGB_BusInit(const char* i2c_path) {
    if(i2c_dev_open(&rgb_device, i2c_path, TCS34725_ADDRESS) != 0) {
        printf("Failed to open RGB sensor\n");
        return -1;
    }

    return 0;
}

void RGB_BusCleanup(void) {
    i2c_dev_close(&rgb_device);
}

/**
 * HELPER FUNCTIONS
**/
static void TCS34725_WriteReg(UBYTE reg, UBYTE val){
	i2c_write_byte(&rgb_device, TCS34725_CMD_BIT | reg, val);
}

static UBYTE TCS34725_ReadReg(UBYTE reg){
	UBYTE val = 0;
    i2c_read_byte(&rgb_device, TCS34725_CMD_BIT | reg, &val);
	return val;
}

static UWORD TCS34725_ReadRegWord(UBYTE reg){
	UWORD val = 0;
    i2c_read_word(&rgb_device, TCS34725_CMD_BIT | TCS34725_CMD_Read_Word | reg, &val);
	return val;
}


/**
 * INIT
**/
UBYTE TCS34725_Init(TCS34725IntegrationTime_t integration_time, TCS34725Gain_t gain){
    UBYTE id;

    id = TCS34725_ReadReg(TCS34725_ID);
    if((id != 0x44) && (id != 0x4D)){
        printf("[ERROR] ID is not 0x44 or 0x4D !!! \r\n");
        return 1;
    }

    TCS34725_WriteReg(TCS34725_ATIME, integration_time);
    TCS34725_WriteReg(TCS34725_CONTROL, gain);

    TCS34725_WriteReg(TCS34725_ENABLE, TCS34725_ENABLE_PON);
    usleep(3 * 1000);   // wait for the device to be ready

    TCS34725_WriteReg(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
    usleep(160 * 1000);


    return 0;

}

/**
 * READ
**/
UWORD TCS34725_ReadClear(void){
    UWORD val;

    val = TCS34725_ReadRegWord(TCS34725_CDATAL);
    return val;
}

// Set the light intensity (0-100%)
void TCS34725_SetLight(UWORD value) {
    if(value > 255){
        value = 255;
    }

    gpioPWM(RGB_LED_PIN, value);
}

/**
 * READ RGB
**/
void TCS34725_ReadRGV(RGB* rgb){
    if(rgb == NULL){
        printf("[ERROR] RGB is NULL !!! \r\n");
        return;
    }

    rgb->C = TCS34725_ReadRegWord(TCS34725_CDATAL);
    rgb->R = TCS34725_ReadRegWord(TCS34725_RDATAL);
    rgb->G = TCS34725_ReadRegWord(TCS34725_GDATAL);
    rgb->B = TCS34725_ReadRegWord(TCS34725_BDATAL);
}
