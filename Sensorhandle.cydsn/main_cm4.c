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
#include <time.h>
#include "my_I2C.h"
#include "sensor.h"
//#include "cy_pdl.h"
//#include "cy_rtc.h"

#define TIMER_PERIOD_MSEC   50U
#define addrADXL (0x53)
//// test variable
//uint16 grader = 0;
//uint16 totalGrader =0;
//uint16 antalomgange = 0;

// Brugt til beregning af graders drejning
// Giver x,y,zAxis[] start værdier
//float xAxis_p1p2[2]={0,0},yAxis_p1p2[2]={0,0},zAxis_p1p2[2]={1,1};
int8 xAxis[2], yAxis[2], zAxis[2];
float xAxis2, yAxis2, zAxis2;
float counter = 0;

void TimerInterruptHandler(void)
{
    Cy_TCPWM_ClearInterrupt(Timer_HW, Timer_CNT_NUM, CY_TCPWM_INT_ON_TC);

    // Her udregnes Xaxis
        xAxis[0] = readRegister(0x32);
        xAxis[1] = readRegister(0x33);
        xAxis2 = (xAxis[0] | xAxis[1] << 8);
        xAxis2 = xAxis2/256;
        
        // Her udregnes Zaxis
        zAxis[0] = readRegister(0x36);
        zAxis[1] = readRegister(0x37);
        zAxis2 = (zAxis[0] | zAxis[1] << 8);
        zAxis2 = zAxis2/256;

        
        // Her udregnes Yaxis
        yAxis[0] = readRegister(0x34);
        yAxis[1] = readRegister(0x35);
        yAxis2 = (yAxis[0] | yAxis[1] << 8);
        yAxis2 = yAxis2/256;
        
        printf(" \tRPM: %.1f \r\n", RPM(xAxis2, yAxis2, zAxis2));
}

int main(void)
{
    UART_Start();
    I2C_Start();
    
    ADXL345Init();
    
    Cy_SysInt_Init(&isrTimer_cfg, TimerInterruptHandler);
    NVIC_EnableIRQ(isrTimer_cfg.intrSrc); /* Enable the core interrupt */ 
    __enable_irq(); /* Enable global interrupts. */
    
    Cy_GPIO_Write(RED_PORT,RED_NUM,1);      // nulstiller RØD LED til debuging og advarsler.
    Cy_GPIO_Write(GREEN_PORT,GREEN_NUM,1);  // nulstiller GRØN LED til debuging og godkendelser.
    setvbuf ( stdin, NULL, _IONBF, 0 );
    setvbuf ( stdout, NULL, _IONBF, 0);
    
    (void)Cy_TCPWM_Counter_Init(Timer_HW, Timer_CNT_NUM, &Timer_config); 
    Cy_TCPWM_Enable_Multiple(Timer_HW, Timer_CNT_MASK); /* Enable the counter instance */
    
    Cy_TCPWM_Counter_SetPeriod(Timer_HW, Timer_CNT_NUM, TIMER_PERIOD_MSEC - 1);
    Cy_TCPWM_TriggerStart(Timer_HW, Timer_CNT_MASK); 

    for (;;)
    {
        // Led sættes så de viser hvis vi oplever fejl
        Cy_GPIO_Write(GREEN_PORT,GREEN_NUM,0);
        Cy_GPIO_Write(RED_PORT,RED_NUM,1);        
//        
//        // Her udregnes Xaxis
//        xAxis[0] = readRegister(0x32);
//        xAxis[1] = readRegister(0x33);
//        xAxis2 = (xAxis[0] | xAxis[1] << 8);
//        xAxis2 = xAxis2/256;
//        xAxis_p1p2[0]=xAxis2;
//        
//        // Her udregnes Zaxis
//        zAxis[0] = readRegister(0x36);
//        zAxis[1] = readRegister(0x37);
//        zAxis2 = (zAxis[0] | zAxis[1] << 8);
//        zAxis2 = zAxis2/256;
//        zAxis_p1p2[0]=zAxis2;
//        
//        // Her udregnes Yaxis
//        yAxis[0] = readRegister(0x34);
//        yAxis[1] = readRegister(0x35);
//        yAxis2 = (yAxis[0] | yAxis[1] << 8);
//        yAxis2 = yAxis2/256;
//        yAxis_p1p2[0]=yAxis2;
//        
//        // Her udskrives de tre Akser
//        //printf("\t Xa: %.1f \tYa: %.1f \tZa: %.1f \r\n", xAxis2, yAxis2, zAxis2);
//        
//        grader = RPM(xAxis2, yAxis2, zAxis2);
//        totalGrader += grader;
//    
//        // for at tælle antal omgange
//        if(totalGrader>=360)
//        {
//            antalomgange++;
//            totalGrader = 0;
//        }
//        
//        //printf(" \tGrader: %d ", grader);
//        //printf("\tAntalRPM:\t%.1f \r\n", RPM(xAxis2, yAxis2, zAxis2));
//        
//        
//        // Her udskrives antal grader drejet og antal omgange
//        printf("Antal grader drejet:\t%d", totalGrader);
//        printf("\tAntal omgange:\t%d \r", antalomgange);
//        printf("\tCounter: \t%.1f \r\n", counter);
    }
    return 0;
}
/* [] END OF FILE */
