/**
 * @file my_I2C.h
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
#define addrADXL (0x53)
#define TIMEOUT 100

/**
 * @brief 
 * 
 */
cy_en_scb_i2c_status_t  status;

/**
 * @brief 
 * 
 */
void waitForOperation(uint8_t);

/**
 * @brief 
 * 
 */
void writeRegister(uint8, int8);

/**
 * @brief 
 * 
 * @return uint8 
 */
uint8 readRegister(uint8);

/* [] END OF FILE */
