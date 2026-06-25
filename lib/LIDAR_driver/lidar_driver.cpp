/**************************************************************
* Class:: CSC-615-01 Spring 2026
* Names:: Justine Tenorio,Alejandro Huang, 
*         Kerry Yu, Wilmaire Mejia, Samantha Chombo-Rodriguez
* Github-Name:: tatinCode
* Project:: Term Project
*
* File:: lidar_driver.cpp
*
* Description::
*
**************************************************************/
#include "lidar_driver.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>

#include "sl_lidar.h"
#include "sl_lidar_driver.h"

struct lidar{
    sl::ILidarDriver *driver;
    sl::IChannel *channel;
    unsigned int timeout_ms;
};

//helper functions
static float node_distance_m(const sl_lidar_response_measurement_node_hq_t *node);
static float node_angle_deg(const sl_lidar_response_measurement_node_hq_t *node);
static int angle_in_range(float angle_deg, float min_deg, float max_deg);
static int distance_blocked(float distance_m, float threshold_m);

extern "C" int lidar_init(lidar_t **out_lidar, const lidar_config_t *config){
    if(out_lidar == NULL || config == NULL || config->serial_port == NULL){
        return -1;
    }

    *out_lidar = NULL;

    lidar_t *lidar = new (std::nothrow) lidar_t{};
    if(lidar == NULL){
        return -2;
    }

    lidar->timeout_ms = config->timeout_ms ? config->timeout_ms : 1000;

    auto channel_result = sl::createSerialPortChannel(config->serial_port, (int)config->baudrate);
    if(!channel_result){
        delete lidar;
        return -3;
    }
    lidar->channel = *channel_result;

    auto driver_result = sl::createLidarDriver();
    if(!driver_result){
        delete lidar->channel;
        delete lidar;
        return -4;
    }
    lidar->driver = *driver_result;

    sl_result result = lidar->driver->connect(lidar->channel);
    if(SL_IS_FAIL(result)){
        lidar_cleanup(&lidar);
        return -5;
    }

    sl_lidar_response_device_info_t device_info;
    result = lidar->driver->getDeviceInfo(device_info, lidar->timeout_ms);
    if(SL_IS_FAIL(result)){
        lidar_cleanup(&lidar);
        return -6;
    }

    sl_lidar_response_device_health_t health_info;
    result = lidar->driver->getHealth(health_info, lidar->timeout_ms);
    if(SL_IS_FAIL(result) || health_info.status == SL_LIDAR_STATUS_ERROR){
        lidar_cleanup(&lidar);
        return -7;
    }

    result = lidar->driver->startScan(false, true, 0, NULL);
    if(SL_IS_FAIL(result)){
        lidar_cleanup(&lidar);
        return -9;
    }

    *out_lidar = lidar;
    return 0;
}

extern "C" int lidar_scan(lidar_t *lidar, lidar_scan_summary_t *out_summary){
    sl_lidar_response_measurement_node_hq_t nodes[8192];

    size_t count = sizeof(nodes) / sizeof(nodes[0]);
    size_t i;

    sl_result result;

    if(lidar == NULL || lidar->driver == NULL || out_summary == NULL){
        return -1;
    }

    //font and sides
    out_summary->front_distance_m = 0.0f;
    out_summary->front_left_distance_m = 0.0f;
    out_summary->front_right_distance_m = 0.0f;

    //sides
    out_summary->left_distance_m = 0.0f;
    out_summary->right_distance_m = 0.0f;

    //back and sides
    out_summary->back_distance_m = 0.0f;
    out_summary->back_left_distance_m = 0.0f;
    out_summary->back_right_distance_m = 0.0f;

    out_summary->point_count = 0;

    result = lidar->driver->grabScanDataHq(nodes, count);

    if(SL_IS_FAIL(result) && result != SL_RESULT_OPERATION_TIMEOUT){
        return -2;
    }

    if(count == 0){
        return 0;
    }

    lidar->driver->ascendScanData(nodes, count);

    {
        float nearest = 0.0f;
        float front = 0.0f;
        float left = 0.0f;
        float right = 0.0f;

        for(i = 0; i < count; i++){
            float distance_m = node_distance_m(&nodes[i]);
            float angle_deg = node_angle_deg(&nodes[i]);

            if(distance_m <= 0.0f){
                continue;
            }

            out_summary->point_count++;

            if(nearest == 0.0f || distance_m < nearest){
                nearest = distance_m;
            }

            if(angle_in_range(angle_deg, 330.0f, 30.0f)){
                if(front == 0.0f || distance_m < front){
                    front = distance_m;
                }
            }

            if(angle_in_range(angle_deg, 60.0f, 120.0f)){
                if(left == 0.0f || distance_m < left){
                    left = distance_m;
                }
            }

            if(angle_in_range(angle_deg, 240.0f, 300.0f)){
                if(right == 0.0f || distance_m < right){
                    right = distance_m;
                }
            }
        }

        out_summary->nearest_distance_m = nearest;
        out_summary->front_distance_m = front;
        out_summary->left_distance_m = left;
        out_summary->right_distance_m = right;

    }

    return 0;
}

extern "C" lidar_obstacle_state lidar_get_obstacle_state(const lidar_scan_summary_t *summary, float threshold_m){
    if(summary == NULL){
        return LIDAR_OBSTACLE_CLEAR;
    }

    if(distance_blocked(summary->front_distance_m, threshold_m)){
        return LIDAR_OBSTACLE_FRONT;
    }

    if(distance_blocked(summary->left_distance_m, threshold_m)){
        return LIDAR_OBSTACLE_LEFT;
    }

    if(distance_blocked(summary->right_distance_m, threshold_m)){
        return LIDAR_OBSTACLE_RIGHT;
    }

    return LIDAR_OBSTACLE_CLEAR;
}

extern "C" void lidar_cleanup(lidar_t **lidar){
    if(lidar == NULL || *lidar == NULL){
        return;
    }

    if((*lidar)->driver != NULL){
        (*lidar)->driver->stop();
        (*lidar)->driver->disconnect();
        delete (*lidar)->driver;
        (*lidar)->driver = NULL;
    }

    if((*lidar)->channel != NULL){
        delete (*lidar)->channel;
        (*lidar)->channel = NULL;
    }

    delete *lidar;
    *lidar = NULL;
}

static float node_distance_m(const sl_lidar_response_measurement_node_hq_t *node){
    return float(node->dist_mm_q2) / 4000.0f;
}

static float node_angle_deg(const sl_lidar_response_measurement_node_hq_t *node){
    return float(node->angle_z_q14) * 90.0f / 16384.0f;
}

static int angle_in_range(float angle_deg, float min_deg, float max_deg){
    if(max_deg >= min_deg){
        return angle_deg >= min_deg && angle_deg <= max_deg;
    }

    return angle_deg >= min_deg || angle_deg <= max_deg;
}

static int distance_blocked(float distance_m, float threshold_m){
    return distance_m > 0.0f && distance_m < threshold_m;
}
