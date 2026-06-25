#pragma once

#include "LINE_sensor/line_sensor.h"
#include "RGB_sensor/rgb_controller.h"
#include "LIDAR_driver/lidar_driver.h"

int sensor_threads_init(volatile int *keep_running);
void sensor_threads_cleanup(void);

int sensor_threads_has_line_reading(void);
LineState sensor_threads_get_line_state(void);

int sensor_threads_has_rgb_reading(void);
DetectedColor sensor_threads_get_color(void);

int sensor_threads_has_lidar_reading(void);
lidar_scan_summary_t sensor_threads_get_lidar_summary(void);

