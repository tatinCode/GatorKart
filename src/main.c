/**************************************************************
 * Class:: CSC-615-01 Spring 2026
 * Names:: Justine Tenorio,Alejandro Huang, 
 *         Kerry Yu, Wilmaire Mejia, Samantha Chombo-Rodriguez
 * Github-Name:: tatinCode
 * Project:: Term Project
 *
 * File:: main.c
 *
 * Description:: 
 **************************************************************/
#include <signal.h>

#include "car_controller.h"

static volatile int keep_running = 1;


void handle_sigint(int sig);

/**
 * main function where program execution begins
 *
 */
int main(void){
	signal(SIGINT, handle_sigint);

    if(init_subsystems(&keep_running) != 0){
        cleanup_subsystems();
        return 1;
    }

    /**
     * main loop for the car controller
     */
    run_main_loop(&keep_running);

    /**
     * cleanups for the car controller
     */
    cleanup_subsystems();

	return 0;
}

// Signal handler for graceful exit on Ctrl+C
void handle_sigint(int sig){
	(void)sig;

	keep_running = 0;
}
