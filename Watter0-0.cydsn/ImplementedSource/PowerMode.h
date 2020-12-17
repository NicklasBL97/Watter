/**
 * @file PowerMode.h
 * @author Nicklas Lind
 * @brief This header includes the functions for adding a FreeRTOS task which can make the system go into deepsleep
 * @version 0.1
 * @date 2020-12-13
 * 
 * 
 * 
 */

#pragma once
#include "project.h"

/**
 * @brief This task can be used to enable deep sleep if not connected
 * The task uses the connState variable to check if the connection state
 * 
 * @param arg unused
 */
void task_gotoDeepSleep(void* arg);

/**
 * @brief The callback function for going to or waking up from deepsleep
 * 
 * @param callbackParams Parameters for the callback function 
 * @param mode The callback mode, indicates if the system is waking up or going to sleep
 * @return cy_en_syspm_status_t the status after the callback, returns CY_SYSPM_SUCCESS on a successful callback
 */
cy_en_syspm_status_t DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);