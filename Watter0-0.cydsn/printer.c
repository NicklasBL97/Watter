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
#include "project.h"
#include "printer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "limits.h"
#include "bleHandler.h"

#define ASCII_ESC 27


SemaphoreHandle_t printerSema;

void printSystemInfo(void* info){
    
    LOG("starting printSystemInfo\n\r");
    
    SystemInfo_t* sysinfo = (SystemInfo_t*)info;
    
    while(1){
        xSemaphoreTake(printerSema,portMAX_DELAY);
        printf( "%c[2J", ASCII_ESC );
        printf( "%c[H", ASCII_ESC );
        printf("System information about Watter:\n\n\r");
        printf("ConnectionState: %s", connState == NOT_CONNECTED ? "NOT CONNECTED\n\r" : "CONNECTED\n\r");
        printf("Current power: %d\r\n",*(sysinfo->effekt));
        printf("Current cadance: %d\r\n",*sysinfo->cadance);
        printf("Current BatteryLevel: %d\r\n",*sysinfo->batterylvl);
        printf("\tX: %d", sysinfo->accData->x);
        printf("\tY: %d", sysinfo->accData->y);
        printf("\tZ: %d\r\n", sysinfo->accData->z);
        
        printf("\nEvents:\r\n");
        xSemaphoreGive(printerSema);
        vTaskDelay(1000);
    }
}

void printer_init(){
    printerSema = xSemaphoreCreateMutex();   
}