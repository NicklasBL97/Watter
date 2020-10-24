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
#include "project.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "limits.h"

#define ARM_MATH_CM4
#include "arm_math.h"

#define DEBUG_MODE

#include "bleHandler.h"
#include "printer.h"

uint8 batterylvl = 100;

SendEffekt_t sendEffectInfo;

SystemInfo systemInformation;

int main(void)
{
    sendEffectInfo.power = 0;
    sendEffectInfo.cadance = 0;
    sendEffectInfo.flags = 0;
    systemInformation.effekt = &sendEffectInfo.power;
    systemInformation.cadance = &sendEffectInfo.cadance;
    systemInformation.batterylvl = &batterylvl;
    __enable_irq();
    
    UART_Start();
    
    setvbuf(stdin,NULL,_IONBF,0);
    setvbuf(stdout,NULL,_IONBF,0);
    
    xTaskCreate(bleTask,"bleTask",2*1024,NULL,1,0);
    xTaskCreate(SendEffekt,"SendEffekt",1*1024,&sendEffectInfo,1,0);
    #ifdef DEBUG_MODE
    xTaskCreate(printSystemInfo,"printSystemInfo",1*1024,&systemInformation,1,0);
    #endif
    xTaskCreate(updateBattery,"updateBattery",1*1024,&batterylvl,1,0);
    vTaskStartScheduler(); //Blocking call, to execute code beyond this line it has to be in the form of a Task
    
    while(1){}
}

/* [] END OF FILE */
