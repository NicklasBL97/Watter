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

int main(void)
{
    __enable_irq();
    
    UART_Start();
    
    setvbuf(stdin,NULL,_IONBF,0);
    setvbuf(stdout,NULL,_IONBF,0);
    printf("System Started\n\r");
    
    xTaskCreate(bleTask,"bleTask",4*1024,NULL,2,0);
    xTaskCreate(SendEffekt,"SendEffekt",1*1024,NULL,1,0);
    vTaskStartScheduler(); //Blocking call, to execute code beyond this line it has to be in the form of a Task
}

/* [] END OF FILE */
