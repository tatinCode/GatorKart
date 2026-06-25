/**************************************************************
* Class:: CSC-615-01 Spring 2026
* Names:: Justine Tenorio,Alejandro Huang, 
*         Kerry Yu, Wilmaire Mejia, Samantha Chombo-Rodriguez
* Github-Name:: tatinCode
* Project:: Term Project
*
* File:: lidar_driver.h
*
* Description::
* 
*
**************************************************************/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lidar lidar_t;

typedef struct{
    const char *serial_port;
    unsigned int baudrate;
    unsigned int timeout_ms;
} lidar_config_t;

typedef struct{
    float nearest_distance_m;

    //front and sides
    float front_distance_m;
    float front_right_distance_m;
    float front_left_distance_m;

    //sides
    float left_distance_m;
    float right_distance_m;

    //back and sides
    float back_distance_m;
    float back_right_distance_m;
    float back_left_distance_m;

    int point_count;
} lidar_scan_summary_t;

typedef enum {
    LIDAR_OBSTACLE_CLEAR,
    LIDAR_OBSTACLE_FRONT,
    LIDAR_OBSTACLE_LEFT,
    LIDAR_OBSTACLE_RIGHT
} lidar_obstacle_state;

int lidar_init(lidar_t **out_lidar, const lidar_config_t *config);
int lidar_scan(lidar_t *lidar, lidar_scan_summary_t *out_summary);
void lidar_cleanup(lidar_t **lidar);
lidar_obstacle_state lidar_get_obstacle_state(const lidar_scan_summary_t *summary, float threshold_m);

#ifdef __cplusplus
}
#endif
