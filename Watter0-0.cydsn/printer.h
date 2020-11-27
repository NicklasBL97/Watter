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

/* [] END OF FILE */
#pragma once
#include "project.h"
#include "bleHandler.h"


typedef struct SystemInfo_t{
    int16* effekt;
    int16* cadance;
    uint8* batterylvl;
}SystemInfo_t;

extern SemaphoreHandle_t printerSema;

#define LOG(...) do { \
    xSemaphoreTake(printerSema,portMAX_DELAY); \
    printf(__VA_ARGS__);   \
    xSemaphoreGive(printerSema); \
} while(0)

//Debug printing over SWD
void printSystemInfo(void* info);
void printer_init();