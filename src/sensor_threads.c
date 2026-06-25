#include "sensor_threads.h"
#include "GPIO_config.h"
#include "config.h"

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

//thread handles

//prototypes for the threads

/**
 * line sensor thread helpers, etc.
 * 
 */
#if ENABLE_LINE_SENSOR

static void *line_thread_main(void *arg);
static volatile LineState line_state;
static volatile int has_line_reading;

#endif

/**
 * lidar sensor thread helpers, etc.
 * 
 */
#if ENABLE_LIDAR

static volatile int has_lidar_reading;
static lidar_t *lidar;
static lidar_scan_summary_t lidar_summary;
static void *lidar_thread_main(void *arg);

#endif

/**
 * rgb sensor thread helpers, etc.
 * 
 */
#if ENABLE_RGB_SENSOR

static volatile DetectedColor color = COLOR_UNKNOWN;
static volatile int has_color_reading;
static void *rgb_thread_main(void *arg);

#endif

//thread handles
static pthread_t line_thread;
static pthread_t lidar_thread;
static pthread_t rgb_thread;
static int rgb_thread_started;
static int line_thread_started;
static int lidar_thread_started;

//thread initializer
int sensor_threads_init(volatile int *keep_running){
    #if ENABLE_LINE_SENSOR
        if(line_sensor_init() != 0){
            return -1;
        }
    
        if(pthread_create(&line_thread, NULL, line_thread_main, (void *)keep_running) != 0){
            line_sensor_cleanup();
            return -1;
        }

        printf("line thread started\n");

        line_thread_started = 1;
    #endif

    #if ENABLE_RGB_SENSOR
        if(rgb_init() != 0){
            return -1;
        }
        if(pthread_create(&rgb_thread, NULL, rgb_thread_main, (void *)keep_running) != 0){
            rgb_cleanup();
            return -1;
        }

        printf("rgb thread started\n");

        rgb_thread_started = 1;
    #endif

    #if ENABLE_LIDAR
        lidar_config_t lidar_config = {
            .serial_port = "/dev/ttyUSB0",
            .baudrate = 115200,
            .timeout_ms = 1000
        };

        if(lidar_init(&lidar, &lidar_config) != 0){
            return -1;
        }

        if(pthread_create(&lidar_thread, NULL, lidar_thread_main, (void *)keep_running) != 0){
            lidar_cleanup(&lidar);
            return -1;
        }

        printf("lidar thread started\n");

        lidar_thread_started = 1;
    #endif

    return 0;
}

//thread cleanup
void sensor_threads_cleanup(void){
    if(line_thread_started){
        pthread_join(line_thread, NULL);
    }

    if(rgb_thread_started){
        pthread_join(rgb_thread, NULL);
    }
    
    if(lidar_thread_started){
        pthread_join(lidar_thread, NULL);
    }

#if ENABLE_LINE_SENSOR
    line_sensor_cleanup();
    line_thread_started = 0;
#endif

#if ENABLE_RGB_SENSOR
    rgb_cleanup();
    rgb_thread_started = 0;
#endif
    
#if ENABLE_LIDAR
    lidar_cleanup(&lidar);
    lidar_thread_started = 0;
#endif
}


#if ENABLE_LINE_SENSOR
/**
 * line sesnsor:
 */
int sensor_threads_has_line_reading(void){
    return has_line_reading;
}

LineState sensor_threads_get_line_state(void){
    return line_state;
}

static void *line_thread_main(void *arg){
    volatile int *keep_running = (volatile int *)arg;
    while(*keep_running){
        line_state = line_sensor_update();
        has_line_reading = 1;
        usleep(SENSOR_POLL_US);
    }

    return NULL;
}

#endif

#if ENABLE_RGB_SENSOR
/**
 * rgb sensor:
 */
int sensor_threads_has_rgb_reading(void){
    return has_color_reading;
}

DetectedColor sensor_threads_get_color(void){
    return color;
}

static void *rgb_thread_main(void *arg){
    volatile int *keep_running = (volatile int *)arg;
    ColorResult color_result;

    while(*keep_running){
        if(rgb_read(&color_result) != 0){
            color = color_result.detected_color;
            has_color_reading = 1;
        }

        usleep(STATUS_POLL_US);
    }

    return NULL;
}
#endif

#if ENABLE_LIDAR
/**
 * lidar:
 */
int sensor_threads_has_lidar_reading(void){

    return has_lidar_reading;
}
lidar_scan_summary_t sensor_threads_get_lidar_summary(void){
    return lidar_summary;
}

static void *lidar_thread_main(void *arg){
    volatile int *keep_running = (volatile int *)arg;
    lidar_scan_summary_t summary;

    while(*keep_running){
        if(lidar_scan(lidar, &summary) == 0){
            ///printf("LIDAR scan points=%d front=%.2f\n", summary.point_count, summary.front_distance_m);
            lidar_summary = summary;
            has_lidar_reading = 1;
        }

        usleep(STATUS_POLL_US);
    }

    return NULL;
}
#endif
