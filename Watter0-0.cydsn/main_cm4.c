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

#include "bleHandler.h"
#include "printer.h"

SystemInfo systemInformation;

int main(void)
{
    __enable_irq();
    
    UART_Start();
    
    setvbuf(stdin,NULL,_IONBF,0);
    setvbuf(stdout,NULL,_IONBF,0);
    printf("System Started\n\r");
    
    systemInformation.effekt = &currentPower;
    systemInformation.cadance = &currentCadance;
    
    xTaskCreate(bleTask,"bleTask",4*1024,NULL,1,0);
    xTaskCreate(SendEffekt,"SendEffekt",1*1024,NULL,1,0);
    xTaskCreate(printSystemInfo,"printSystemInfo",1*1024,&systemInformation,1,0);
    vTaskStartScheduler(); //Blocking call, to execute code beyond this line it has to be in the form of a Task
    
    while(1){}
}

/* [] END OF FILE */
