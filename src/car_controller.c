/**************************************************************
 * Class:: CSC-615-01 Spring 2026
 * Names:: Justine Tenorio,Alejandro Huang, 
 *         Kerry Yu, Wilmaire Mejia, Samantha Chombo-Rodriguez
 * Github-Name:: tatinCode
 * Project:: Term Project
 *
 * File:: car_controller.c
 *
 * Description:: 
 * This file contains the main loop for the car controller
 * It is responsible for initializing the subsystems,
 * running the main loop, and cleaning up the subsystems
 *
 **************************************************************/
#include "sensor_threads.h"
#include "config.h"
#include "GPIO_config.h"
#include "BUTTON/button.h"

#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <pigpio.h>

//includes for the drivers
#if ENABLE_RGB_SENSOR
#include "RGB_sensor/rgb_controller.h"
#endif

#if ENABLE_LIDAR
#include "LIDAR_driver/lidar_driver.h"
#endif

#if ENABLE_LINE_SENSOR
#include "LINE_sensor/line_sensor.h"
#endif

#if ENABLE_MOTORS
//includes go here
#include "MOTOR_driver/motor_driver.h"
#endif

/**
 * prototypes
 *
 */
#if ENABLE_RGB_SENSOR
static int handle_color_detection(void);

static int red_stop = 0;
#endif

#if ENABLE_LIDAR
static int handle_obstacle_detection(void);
#endif

#if ENABLE_LINE_SENSOR && ENABLE_MOTORS
static void handle_line_following(void);

static MotorCommands last_turn_command = MOTOR_PIVOT_RIGHT;
static int turn_90 = 0;
#endif
 
//enum for the color blue's state
typedef enum {
	BLUE_READY, 
	BLUE_USED
}ColorBlueState;

typedef enum {
	AVOID_NONE, 
    AVOID_STOP,
	AVOID_SHIFT_LEFT,
	AVOID_LEFT_CLEAR_DELAY,
	AVOID_FORWARD,
	AVOID_RIGHT_DELAY,
    AVOID_SHIFT_RIGHT_TO_LINE
}AvoidState;

static int pigpio_started = 0;

int init_subsystems(volatile int *keep_running){
    gpioCfgSetInternals(PI_CFG_NOSIGHANDLER);

    if (gpioInitialise() < 0) {
        fprintf(stderr, "Failed to initialize pigpio.\n");
        return -1;
    }

    if (button_init() != 0){
        return -1;
    }

    while (*keep_running && !button_pressed()){
        usleep(10* 1000);
    }

    pigpio_started = 1;

	#if ENABLE_MOTORS
		motor_init();
		printf("Motors init\n");
	#endif

    #if ENABLE_LINE_SENSOR || ENABLE_RGB_SENSOR || ENABLE_LIDAR
        if (sensor_threads_init(keep_running) != 0) {
            return -1;
        }
    #endif

    return 0;
}

void run_main_loop(volatile int *keep_running){

    int color_override = 0;
    int obstacle_override = 0;
    
    #if ENABLE_MOTORS
        motor_update(MOTOR_FORWARD);
    #endif

    while(*keep_running){          //start while loop

    #if ENABLE_LIDAR 
        //lidar_update
        obstacle_override = handle_obstacle_detection();
    #endif

    #if ENABLE_RGB_SENSOR && ENABLE_MOTORS
    		//rgb_update
            if(!obstacle_override){
                color_override = handle_color_detection();
            }
    #endif
    
    
    #if ENABLE_LINE_SENSOR && ENABLE_MOTORS
            if(!color_override && !obstacle_override){
                handle_line_following();
            }
    #endif

            //delay for the loop: 20ms
            //usleep(10 * 1000);
    }                           //end while loop

}

void cleanup_subsystems(void){
	//cleanups
	#if ENABLE_RGB_SENSOR || ENABLE_LIDAR || ENABLE_LINE_SENSOR
	    sensor_threads_cleanup();
	#endif
	
	#if ENABLE_MOTORS
	    motor_cleanup();
	#endif

        if(pigpio_started) {
            gpioTerminate();
            pigpio_started = 0;
        }
}

#if ENABLE_LIDAR

static AvoidState avoid_state = AVOID_NONE;

static int is_blocked(float distance_m){
    return distance_m > 0.0f && distance_m < LIDAR_FRONT_STOP;
}

static int is_clear(float distance_m){
    return distance_m == 0.0f || distance_m >= LIDAR_FRONT_STOP;
}

static int handle_obstacle_detection(void){
    lidar_scan_summary_t lidar_summary;

    if(!sensor_threads_has_lidar_reading()){
        return avoid_state != AVOID_NONE;
    }

    lidar_summary = sensor_threads_get_lidar_summary();

//    printf("LIDAR points=%d, nearest=%.2f, front=%.2f, left=%.2f, right=%.2f\n",
//            lidar_summary.point_count,
//            lidar_summary.nearest_distance_m,
//            lidar_summary.front_distance_m,
//            lidar_summary.left_distance_m,
//            lidar_summary.right_distance_m);
    #if ENABLE_MOTORS && ENABLE_LINE_SENSOR
        LineState line_state = sensor_threads_get_line_state();
    #endif

    #if ENABLE_MOTORS
        switch(avoid_state){
            case AVOID_NONE:
                if(is_blocked(lidar_summary.front_distance_m)){
                    printf("Obstacle detected at %.2fm: stopping\n", lidar_summary.front_distance_m);
                    motor_update(MOTOR_STOP);
                    usleep(2 * 1000 * 1000);
                    avoid_state = AVOID_SHIFT_LEFT;
                    return 1;
                }
                return 0;

            case AVOID_SHIFT_LEFT:
                if(is_blocked(lidar_summary.front_distance_m)){
                    motor_update(MOTOR_SHIFT_LEFT);
                    return 1;
                }

                usleep(100 * 1000);
                motor_update(MOTOR_STOP);
                usleep(300 * 1000);
                motor_update(MOTOR_FORWARD);
                avoid_state = AVOID_FORWARD;
                usleep(500 * 1000);
                return 1;

            case AVOID_FORWARD:
                usleep(2 * 1000 * 1000);
                if(is_blocked(lidar_summary.right_distance_m)){
                    motor_update(MOTOR_FORWARD);
                    return 1;
                }

                motor_update(MOTOR_FORWARD);
                usleep(70 * 1000);
                motor_update(MOTOR_STOP);
                avoid_state = AVOID_SHIFT_RIGHT_TO_LINE;
                return 1;

            case AVOID_SHIFT_RIGHT_TO_LINE:{

                if(line_state != LINE_STATE_CENTERED && line_state != LINE_STATE_INTERSECTION){
                    motor_update(MOTOR_SHIFT_RIGHT);
                    return 1;
                }

                motor_update(MOTOR_STOP);
                usleep(150 * 1000);
                avoid_state = AVOID_NONE;
                return 1;
					   }

            default:
                motor_update(MOTOR_STOP);
                avoid_state = AVOID_NONE;
                return 1;
        }
    #endif

    return avoid_state != AVOID_NONE;

}
#endif

#if ENABLE_RGB_SENSOR && ENABLE_MOTORS 
static int handle_color_detection(void){
    static ColorBlueState blue_state;
    DetectedColor detected_color;

    if(!sensor_threads_has_rgb_reading()){
        return 0;
    }

    if(red_stop){
	    motor_update(MOTOR_STOP);
	    return 1;
    }

    detected_color = sensor_threads_get_color();

    switch(detected_color){
	case COLOR_DARK_RED:
        case COLOR_RED:
            printf("RED\n");
            red_stop = 1;
            motor_update(MOTOR_STOP);
            return 1;

	case COLOR_DARK_BLUE:
	case COLOR_DARKER_BLUE:
        case COLOR_BLUE:
            printf("BLUE \n");
            if (blue_state == BLUE_READY) {
                blue_state = BLUE_USED;
                motor_update(MOTOR_STOP);
                usleep(5 * 1000 * 1000);
                motor_update(MOTOR_FORWARD);
                usleep(200 * 1000);
            }
            return 1;
            
        case COLOR_BLACK:
            printf("BLACK\n");
            blue_state = BLUE_READY;
            return 0;

        default:
            //blue_state= BLUE_READY;
            printf("default\n");
            return 0;
    }
}
#endif

#if ENABLE_LINE_SENSOR && ENABLE_MOTORS
static void handle_line_following(void){
    LineState line_state = sensor_threads_get_line_state();

    printf("Line State: %d\n", line_state);

    switch (line_state) {
        case LINE_STATE_CENTERED:
            motor_update(MOTOR_FORWARD);
            break;


        case LINE_STATE_LIGHT_LEFT:
	    motor_update(MOTOR_ADJUST_LEFT);
	    printf("adjust left\n");
	    
	    break;
        case LINE_STATE_HARD_LEFT:
            motor_update(MOTOR_PIVOT_LEFT);
            while  (sensor_threads_get_line_state() == LINE_STATE_HARD_LEFT) {
		usleep(5000);
	    }
	    printf("hard left\n");
//	    motor_update(MOTOR_FORWARD);
//	    motor_update(MOTOR_FORWARD);
	    break;

        case LINE_STATE_LIGHT_RIGHT:
	    motor_update(MOTOR_ADJUST_RIGHT);
	    printf("light right\n");
	    break;
        case LINE_STATE_HARD_RIGHT: 
            motor_update(MOTOR_ADJUST_RIGHT);
	   while (sensor_threads_get_line_state() == LINE_STATE_HARD_RIGHT) {
                usleep(5000);
            }
	    printf("hard right\n");
//            motor_update(MOTOR_FORWARD);
//	    motor_update(MOTOR_FORWARD);
	    break;

        case LINE_STATE_INTERSECTION:
            motor_update(MOTOR_FORWARD);
            break;

        case LINE_STATE_LOST_LINE:
        case LINE_STATE_UNDEFINED:
        default:
            motor_update(MOTOR_FORWARD);
            break;

    }
}
#endif
