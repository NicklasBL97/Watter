/**
 * @file printer.h
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
#include "bleHandler.h"
#include "stdio.h"
#include "ADXL345Sensor.h"

/**
 * @brief 
 * 
 */
typedef struct SystemInfo_t{
    int16* effekt;
    int16* cadance;
    uint8* batterylvl;

}SystemInfo_t;

/**
 * @brief 
 * 
 */
extern SemaphoreHandle_t printerSema;

/**
 * @brief 
 * 
 */
#define LOG(...) do { \
    xSemaphoreTake(printerSema,portMAX_DELAY); \
    printf(__VA_ARGS__);   \
    xSemaphoreGive(printerSema); \
} while(0)

/**
 * @brief 
 * 
 * @param info 
 */
void printSystemInfo(void* info);

/**
 * @brief 
 * 
 */
void printer_init();