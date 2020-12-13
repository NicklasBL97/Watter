/**
 * @file Battery.h
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
 */
typedef struct {
    uint16 BatteryVoltage;
    uint8 batterylvl;
}Battery_t;

/**
 * @brief 
 * 
 * @param battery 
 */
void BatteryInit(Battery_t* battery);