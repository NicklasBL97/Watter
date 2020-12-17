/**
 * @file printer.h
 * @author Nicklas Lind
 * @brief This header includes a LOG macro for printing to stdout and a FreeRTOS task which periodically writes information about the system to a UART
 * @version 0.1
 * @date 2020-12-13
 * 
 * 
 * 
 */
#pragma once
#include "project.h"
#include "bleHandler.h"
#include "stdio.h"
#include "ADXL345Sensor.h"

/**
 * @brief This struct is used to point at different variables of interrest. Variables can then be printed with printSystemInfo()
 * 
 * 
 */
typedef struct SystemInfo_t{
    int16* effekt;
    int16* cadance;
    uint8* batterylvl;

}SystemInfo_t;

/**
 * @brief This semaphore is used to lock whenever there is printed to the UART see LOG(...)
 * 
 */
extern SemaphoreHandle_t printerSema;

/**
 * @brief This macro is used to write to stdout. The macro locks the printerSema semaphore
 * 
 */
#define LOG(...) do { \
    xSemaphoreTake(printerSema,portMAX_DELAY); \
    printf(__VA_ARGS__);   \
    xSemaphoreGive(printerSema); \
} while(0)

/**
 * @brief This is a FreeRTOS task made to print information about the system, all information is pointed to in a SystemInfo_t struct
 * 
 * @param info a pointer to a SystemInfo_t struct containing pointers to variables of interrest
 */
void printSystemInfo(void* info);

/**
 * @brief Initialiser function used to initialise the semaphore
 * 
 */
void printer_init();