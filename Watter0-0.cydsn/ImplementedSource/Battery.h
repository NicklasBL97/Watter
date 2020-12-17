/**
 * @file Battery.h
 * @author Nicklas Lind
 * @brief This header includes a battery struct where information about the battery voltage and percentage charge can be stored
 * @version 0.1
 * @date 2020-12-13
 * 
 * 
 * 
 */

#pragma once
#include "project.h"

/**
 * @brief This struct can be used to store information about a battery voltage and percentage charge
 * 
 */
typedef struct {
    uint16 BatteryVoltage;
    uint8 batterylvl;
}Battery_t;

/**
 * @brief Initialisation function for a Battery_t struct
 * 
 * @param battery The battery struct to initialise
 */
void BatteryInit(Battery_t* battery);