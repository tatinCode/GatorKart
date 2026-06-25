/**************************************************************
 * Class:: CSC-615-01 Spring 2026
 * Names:: Justine Tenorio,Alejandro Huang, 
 *         Kerry Yu, Wilmaire Mejia, Samantha Chombo-Rodriguez
 * Github-Name:: tatinCode
 * Project:: Term Project
 *
 * File:: car_controller.h
 *
 * Description:: 
 * This file contains the main loop for the car controller
 * It is responsible for initializing the subsystems,
 * running the main loop, and cleaning up the subsystems
 *
 **************************************************************/
#pragma once

#define AVOID_STOP_US 

int init_subsystems(volatile int *keep_running);
void run_main_loop(volatile int *keep_running);
void cleanup_subsystems();
