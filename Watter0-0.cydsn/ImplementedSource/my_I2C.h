/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#pragma once
#include "project.h"
#define addrADXL (0x53)
#define TIMEOUT 100

cy_en_scb_i2c_status_t  status;

void waitForOperation(uint8_t);
void writeRegister(uint8, int8);
uint8 readRegister(uint8);

/* [] END OF FILE */