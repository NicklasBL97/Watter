/**
 * @file PowerMode.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#include "project.h"

/**
 * @brief 
 * 
 * @param arg 
 */
void task_gotoDeepSleep(void* arg);

/**
 * @brief 
 * 
 * @param callbackParams 
 * @param mode 
 * @return cy_en_syspm_status_t 
 */
cy_en_syspm_status_t TCPWM_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);