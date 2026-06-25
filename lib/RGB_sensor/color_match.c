/**************************************************************
* Class:: CSC-615-01 Spring 2026
* Names:: Justine Tenorio,Alejandro Huang, 
*         Kerry Yu, Wilmaire Mejia, Samantha Chombo-Rodriguez
* Github-Name:: tatinCode
* Project:: Term Project
*
* File:: color_match.c
* Description:: This file contains our color references 
*  and the implementation of color matching. It includes a 
*  reference set of colors and function to normalize
*  the RGB values and find the nearest color match.
*
**************************************************************/
#include "color_match.h"

#include <float.h>
#include <unistd.h>
#include <stdio.h>

// Predefined color references (normalized RGB values)
typedef struct{
    DetectedColor detected_color;

    double r;
    double g;
    double b;
} ColorRef;

static const ColorRef color_refs[] = {
    //black is separated cause they're a special case 1
    {COLOR_RED,0.479, 0.271, 0.249},
    {COLOR_DARK_RED,0.506, 0.258, 0.232},
    {COLOR_BLUE,0.334, 0.333, 0.331},
    {COLOR_DARK_BLUE, 0.348, 0.336, 0.315},
    {COLOR_DARKER_BLUE, 0.343, 0.336, 0.320},
    //{COLOR_WHITE,0.391,0.351,0.256},
    {COLOR_GREEN, 0.343, 0.374, 0.263},
    {COLOR_YELLOW, 0.415, 0.360, 0.225}, 
    {COLOR_ORANGE, 0.480, 0.286, 0.235},
    {COLOR_LIGHT_BLUE, 0.359, 0.354, 0.294},
    {COLOR_BROWN, 0.443,0.312,0.244}
};

static const int color_ref_count = sizeof(color_refs) / sizeof(color_refs[0]);

// Helper function to convert normalized RGB to 8-bit values
static unsigned int ToByte(double value){
    if(value <= 0.0){
        return 0;
    }

    if(value >= 1.0){
        return 255;
    }

    return (unsigned int)(value * 255.0 + 0.5);
}

/**
 * NearestColor():
 * 
 * Helper function to find the nearest color reference
 * based on normalized RGB values
 * 
 * @param r
 * @param g 
 * @param b 
 * @return 
 */
static DetectedColor NearestColor(double r, double g, double b){
    int i;
    double best_dist = DBL_MAX;

    DetectedColor best_color = COLOR_UNKNOWN;

    // Loop through all color references
    // and find the nearest one
    for(i = 0; i < color_ref_count; i++){
        double dr = r - color_refs[i].r;
        double dg = g - color_refs[i].g;
        double db = b - color_refs[i].b;
        double dist = (dr * dr) + (dg * dg) + (db * db);

        // If the distance is less than the best distance
        // so far, update the best distance and best name
        // and continue to the next color reference
        if(dist < best_dist){
            best_dist = dist;
            best_color = color_refs[i].detected_color;
        }
    }

    return best_color;
}

// Main function to analyze the color based on RGB values
int AnalyzeColor(const RGB *rgb, ColorResult *result){
    unsigned int sum;
    unsigned int r8;
    unsigned int g8;
    unsigned int b8;

    double r= 0.0;
    double g= 0.0;
    double b= 0.0;
    double c= 0.0;

    if(rgb == NULL || result == NULL){
        return 0;
    }


    sum = rgb->R + rgb->G + rgb->B;

    if(sum > 0){
        r = (double)rgb->R / sum;
        g = (double)rgb->G / sum;
        b = (double)rgb->B / sum;
	c = (double)rgb->C;
    }

    result->r_norm = r;
    result->g_norm = g;
    result->b_norm = b;
    result->c_norm = c;

    r8 = ToByte(r);
    g8 = ToByte(g);
    b8 = ToByte(b);

    result->rgb_hex = (r8 << 16) | (g8 << 8) | b8;

    //debug prints for calibration
    printf("Red: %f, Green: %f, Blue: %f, Clear: %d\n", r, g, b, rgb->C);

    if(rgb->C > WHITE_THRESHOLD){
	    result->detected_color = COLOR_WHITE;
	    printf("White\n");
	    return 1;
    }

    if(rgb->C < BLACK_THRESHOLD){
        result->detected_color = COLOR_BLACK;
	    printf("Black\n");
        return 1;
    }

    result->detected_color = NearestColor(r, g, b);
    switch(result->detected_color){
        case COLOR_RED:
            printf("RED\n");
            break;

        case COLOR_DARK_RED: 
            printf("DARK RED\n");
            break;
	    
        case COLOR_BLUE:
            printf("BLUE \n");
            break;

        case COLOR_DARK_BLUE:
            printf("DAKR BLUE \n");
            break;

        case COLOR_DARKER_BLUE:
            printf("DAKRER BLUE \n");
            break;

        case COLOR_BLACK:
            printf("BLACK\n");
            break;

        case COLOR_GREEN:
            printf("GREEN\n");
            break;

        case COLOR_YELLOW:
            printf("YELLOW\n");
            break;

        case COLOR_ORANGE:
            printf("ORANGE\n");
            break;
        case COLOR_BROWN:
            printf("BROWN\n");
            break;

        case COLOR_LIGHT_BLUE: 
            printf("LIGHT BLUE\n");
            break;

        default:
            printf("default\n");
            break;
    }
    return 1;
}

// Reads multiple RGB samples and averages them for a more stable reading
void ReadAvgRGB(RGB *rgb, int samples, int delay_ms){
    RGB sample;
    unsigned int tot_r = 0;
    unsigned int tot_g = 0;
    unsigned int tot_b = 0;
    unsigned int tot_c = 0;
    int i;

    if(rgb == NULL || samples <= 0){
        printf("[ERROR] Invalid parameters !!! \r\n");
        return;
    }
    
    for(i = 0; i < samples; i++){
        TCS34725_ReadRGV(&sample);

        tot_r += sample.R;
        tot_g += sample.G;
        tot_b += sample.B;
        tot_c += sample.C;
        
        if(i < samples - 1){
            usleep(delay_ms * 1000);
        }
    }

    rgb->R = tot_r / samples;
    rgb->G = tot_g / samples;
    rgb->B = tot_b / samples;
    rgb->C = tot_c / samples;
}

