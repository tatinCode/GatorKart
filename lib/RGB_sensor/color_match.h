

#pragma once

#include "RGB_Sensor.h"

#define DARK_THRESHOLD 200U
#define BLACK_THRESHOLD 520U
#define WHITE_THRESHOLD 18000U
#define UNKNOWN_DISTANCE 0.050

//enum for color detection
typedef enum{
    COLOR_RED,
    COLOR_DARK_RED,
    COLOR_BLUE,
    COLOR_DARK_BLUE,
    COLOR_DARKER_BLUE,
    COLOR_BLACK,
    COLOR_UNKNOWN,
    COLOR_WHITE,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_ORANGE,
    COLOR_LIGHT_BLUE,
    COLOR_BROWN
} DetectedColor;

// Struct to hold the result of color analysis
typedef struct{
    double r_norm;
    double g_norm;
    double b_norm;
    double c_norm;

    unsigned int rgb_hex;
    const char* color_name;

    DetectedColor detected_color;
} ColorResult;


// Function prototypes
int AnalyzeColor(const RGB *rgb, ColorResult *result);
void ReadAvgRGB(RGB *rgb, int samples, int delay_ms);
