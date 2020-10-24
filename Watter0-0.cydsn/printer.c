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
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "limits.h"

#define ASCII_ESC 27

void printSystemInfo(void* info){
    printf("starting printSystemInfo\n\r");
    SystemInfo* sysinfo = (SystemInfo*)info;
    while(1){
        printf( "%c[2J", ASCII_ESC );
        printf( "%c[H", ASCII_ESC );
        printf("System information about Watter:\n\n\r");
        printf("Current power: %d\r\n",*sysinfo->effekt);
        printf("Current cadance: %d\r\n",*sysinfo->cadance);
        printf("Current BatteryLevel: %d\r\n",*sysinfo->batterylvl);
        
        vTaskDelay(1000);
    }
}