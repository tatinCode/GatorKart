#include <pigpio.h>
#include "button.h"

#define BUTTON_PIN 21

int button_init(void) {

    gpioSetMode(BUTTON_PIN, PI_INPUT);
    gpioSetPullUpDown(BUTTON_PIN, PI_PUD_DOWN);
    return 0;
}

int button_pressed(void){
    return gpioRead(BUTTON_PIN)== 1 ;
}
