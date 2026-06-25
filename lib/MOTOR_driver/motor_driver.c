/* **************************************************************
 * Class:: CSC-615-01 Spring 2026
 * Name:: Samantha Chombo-Rodriguez
 * Github-Name:: tatinCode
 * Project:: Term Project
 *
 * File:: motor_driver.c        
 *
 * Description:: Uses the BCM2835 library to read the button  and start motor
 * using PCA9685
 *
 **************************************************************/
#include "MOTOR_driver/motor_driver.h"

#include <stdio.h>
#include <unistd.h>

#define HAT1 0x43 // top hat 
#define HAT2 0x40 // bottom hat

#define MOTOR_FREQUENCY 1000

static I2CDevice hat1 = { .addr = HAT1};
static I2CDevice hat2 = { .addr = HAT2};

#define BASE_SPEED 70

static const  MotorChannels MOTOR_A = {PCA_CHANNEL_0, PCA_CHANNEL_1, PCA_CHANNEL_2};
static const  MotorChannels MOTOR_B = {PCA_CHANNEL_5, PCA_CHANNEL_3,PCA_CHANNEL_4};

void motor_backward(I2CDevice *hataddy, MotorChannels motor, int speed) {

	PCA9685_SetLevel(hataddy, motor.in1,1);
	PCA9685_SetLevel(hataddy, motor.in2,0);
	PCA9685_SetPwmDutyCycle(hataddy, motor.pwm,speed);
}
void motor_forward(I2CDevice *hatAddy, MotorChannels motor, int speed) {

        PCA9685_SetLevel(hatAddy, motor.in1, 0);
        PCA9685_SetLevel(hatAddy, motor.in2, 1);
        PCA9685_SetPwmDutyCycle(hatAddy, motor.pwm, speed);
}

void  all_motor_forward(int speed) {

	motor_forward(&hat2,MOTOR_A,speed);
	motor_forward(&hat2,MOTOR_B,speed);
	motor_forward(&hat1,MOTOR_A,speed);
	motor_forward(&hat1,MOTOR_B,speed);
}


void motor_stop(I2CDevice *hataddy, MotorChannels motor) { 
	PCA9685_SetPwmDutyCycle(hataddy, motor.pwm, 0);
}

void all_motor_backwards(int speed) {
        motor_backward(&hat2, MOTOR_A,speed);
        motor_backward(&hat2, MOTOR_B,speed);
        motor_backward(&hat1,MOTOR_A,speed);
        motor_backward(&hat1, MOTOR_B,speed);
}

/**
 * Bottom hat - HAT 1: front
 *
 * Front left: MA
 * Front Right: MB
 *
 *
 * Top hat - HAT 2: back
 *
 * Back left: MA
 * Back right: MB
 *
 *
 */
void  motor_left(int speed){
    // back left
    motor_backward(&hat2, MOTOR_A, (speed * 0.7));

    // back right
    motor_forward(&hat2,MOTOR_B,  (speed * 1.0));

    // front left
    motor_backward(&hat1,MOTOR_A, (speed * 0.6));

    //front right
    motor_forward(&hat1,MOTOR_B,  (speed * 0.9));
}

void motor_right(int speed) {
	// back left
        motor_forward(&hat2, MOTOR_A, (speed * 1.0));

	// back right
        motor_backward(&hat2, MOTOR_B,(speed * 0.7));

	// front left
        motor_forward(&hat1, MOTOR_A,  (speed * 0.9));

	// front right
        motor_backward(&hat1, MOTOR_B, (speed * 0.6));
}

void motor_adjust_left(int speed) {
	// back left
	motor_forward(&hat2, MOTOR_A, (speed * 0.4));
    // front left
    motor_forward(&hat1, MOTOR_A, (speed * 0.4));

	// back right
	motor_forward(&hat2, MOTOR_B, (speed * 1));
    // front right
    motor_forward(&hat1, MOTOR_B, (speed * 1));
}

void motor_adjust_right(int speed) {
	// back left
	motor_forward(&hat2, MOTOR_A, (speed * 1));
    // front left
    motor_forward(&hat1, MOTOR_A, (speed * 1));

	// back right
	motor_forward(&hat2, MOTOR_B, (speed * 0.4));
    // front right
    motor_forward(&hat1, MOTOR_B, (speed * 0.4));
}

void motor_pivot_right(int speed) {
        // back left
        motor_forward(&hat2, MOTOR_A, (speed * 1.0));
        //motor_forward(&hat2, MOTOR_A,speed);

        // back right
        motor_backward(&hat2, MOTOR_B, (speed * 0.8));
        //motor_backward(&hat2, MOTOR_B,speed);

        // front left
        motor_forward(&hat1, MOTOR_A, (speed * 1.0));
        //motor_forward(&hat1, MOTOR_A,speed);

        // front right
        motor_backward(&hat1, MOTOR_B,(speed * 0.8));
        //motor_backward(&hat1, MOTOR_B,speed);
}

void  motor_pivot_left(int speed){
        // back left
        motor_backward(&hat2, MOTOR_A, (speed * 0.8));
        //motor_backward(&hat2,MOTOR_A, speed);

        // back right
        motor_forward(&hat2,MOTOR_B, (speed * 1));
        //motor_forward(&hat2,MOTOR_B, speed);

        // front left
        motor_backward(&hat1,MOTOR_A, (speed * 0.8));
        //motor_backward(&hat1,MOTOR_A, speed);

        //front right
        motor_forward(&hat1,MOTOR_B, (speed * 1));
        //motor_forward(&hat1,MOTOR_B, speed);
}

void reverse_right(int speed) {
        motor_backward(&hat2, MOTOR_A,speed);
        motor_backward(&hat2, MOTOR_B,0);
        motor_backward(&hat1, MOTOR_A, speed);
}     

/**
 * shifting
 * A: left
 * B: right
 * HAT 1: front
 * HAT 2: back
 */
void shift_left(int speed) {
    motor_backward(&hat1, MOTOR_A, speed);
    motor_backward(&hat1, MOTOR_B, speed);
    motor_forward(&hat2, MOTOR_A, speed * 0.92);
    motor_forward(&hat2, MOTOR_B, speed * 0.88);
}

void shift_right(int speed) {
    motor_forward(&hat1, MOTOR_A, speed);
    motor_forward(&hat1, MOTOR_B, speed);
    motor_backward(&hat2, MOTOR_A, speed * 0.92);
    motor_backward(&hat2, MOTOR_B, speed * 0.88);
}

void all_stop () {
	motor_stop(&hat1, MOTOR_A);
	motor_stop(&hat1, MOTOR_B);
	motor_stop(&hat2, MOTOR_A);
	motor_stop(&hat2, MOTOR_B);
}

void motor_init() {
    if(i2c_dev_open(&hat1, "/dev/i2c-1", HAT1) != 0) {
        printf("Failed to open &hat1\n");
        return;
    }

    if(i2c_dev_open(&hat2, "/dev/i2c-1", HAT2) != 0) {
        printf("Failed to open &hat2\n");
        i2c_dev_close(&hat1);
        return;
    }

    PCA9685_Init(&hat1);
    PCA9685_Init(&hat2);

    PCA9685_SetPWMFreq(&hat1, MOTOR_FREQUENCY);
    PCA9685_SetPWMFreq(&hat2, MOTOR_FREQUENCY);

  // Using the I2C
   usleep(2 * 1000 * 1000);

}


void motor_cleanup(void) {
  all_stop();
  i2c_dev_close(&hat1);
  i2c_dev_close(&hat2);
}

void motor_update(MotorCommands motorCommand) {
    //printf("motorCommand: %d\n", motorCommand);
    switch(motorCommand) {
        case MOTOR_FORWARD:
            all_motor_forward(BASE_SPEED);
            break;
        case MOTOR_BACKWARD:
            all_motor_backwards(BASE_SPEED);
            break;
        case MOTOR_LEFT:
            motor_left(BASE_SPEED);
            break;
        case MOTOR_RIGHT:
            motor_right(BASE_SPEED);
            break;
        case MOTOR_STOP:
            all_stop();
            break;
        case MOTOR_PIVOT_RIGHT:
            motor_right(BASE_SPEED);
	    break;
        case MOTOR_PIVOT_LEFT:
            motor_left(BASE_SPEED);
            break;
        case MOTOR_ADJUST_RIGHT:
            motor_adjust_right(BASE_SPEED);
            break;
        case MOTOR_ADJUST_LEFT:
            motor_adjust_left(BASE_SPEED);
            break;
        case MOTOR_SHIFT_LEFT:
            shift_left(BASE_SPEED);
            break;
        case MOTOR_SHIFT_RIGHT:
            shift_right(BASE_SPEED);
            break;
    }
}
