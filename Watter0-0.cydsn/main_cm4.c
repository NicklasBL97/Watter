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
#include "queue.h"
#include "limits.h"
#include "ImplementedSource/my_I2C.h"
#include "ImplementedSource/ADXL345Sensor.h"
#include "ImplementedSource/Battery.h"
#include "ImplementedSource/PowerMode.h"

#define DEBUG_MODE

#include "ImplementedSource/bleHandler.h"
#include "ImplementedSource/printer.h"

float test;
uint32 dst[2];
Battery_t battery;
SystemInfo_t systemInformation;
ADXL345Data accelerometerData;


void ADC_ISR_Callback(void){
    sendEffectInfo.power = ADC_CountsTo_mVolts(0,dst[0]);
    battery.BatteryVoltage = ADC_CountsTo_mVolts(1,dst[1]);
    
    Cy_DMA_Channel_ClearInterrupt(DMA_Sample_HW, 0);
}

typedef struct AccelerometerData{
    int16 x;
    int16 y;
    int16 z;
}AccelerometerData;

void Cad_ISR_Callback(void){
    accelerometerData = ADXL345GetData();
    sendEffectInfo.cadance = (int16)RPM(accelerometerData.x, accelerometerData.y, accelerometerData.z);
}

int main(void)
{
    //enable DMA for getting samples from the ADC sampling on the instrumental amplifier and battery
    DMA_Sample_Start((uint32_t *) &(SAR->CHAN_RESULT[0]),&dst[0]);
    Cy_DMA_Channel_SetInterruptMask(DMA_Sample_HW,0,CY_DMA_INTR_MASK);
    
    //Start subsystems and hardware blocks
    SendEffekt_init();
    printer_init();
    BatteryInit(&battery);
    I2C_Start();
    ADXL345Init();
    ADC_Start();
    ADC_StartConvert();
    UART_Start();
    
    systemInformation.effekt = &sendEffectInfo.power;
    systemInformation.cadance = &sendEffectInfo.cadance;
    systemInformation.batterylvl = &battery.batterylvl;
    
    __enable_irq();
    Cy_SysInt_Init(&Sample_Int_cfg,ADC_ISR_Callback); 
    NVIC_ClearPendingIRQ(Sample_Int_cfg.intrSrc);
	NVIC_EnableIRQ(Sample_Int_cfg.intrSrc);
    
    Cy_SysInt_Init(&Cad_int_cfg,Cad_ISR_Callback); 
    NVIC_ClearPendingIRQ(Cad_int_cfg.intrSrc);
	NVIC_EnableIRQ(Cad_int_cfg.intrSrc);
    

    //initialize timer for periodic sampling from I2C sensor
    Cy_TCPWM_Counter_Init(Timer_Cad_Sample_HW, Timer_Cad_Sample_CNT_NUM,&Timer_Cad_Sample_config);
    Cy_TCPWM_Enable_Multiple(Timer_Cad_Sample_HW, Timer_Cad_Sample_CNT_MASK);
    Cy_TCPWM_TriggerStart(Timer_Cad_Sample_HW,Timer_Cad_Sample_CNT_MASK);
    
    
    //No IO buffering, write when something is put in stdout
    setvbuf(stdin,NULL,_IONBF,0);
    setvbuf(stdout,NULL,_IONBF,0);
    
    //Create FreeRTOS tasks 
    xTaskCreate(task_ble,"bleTask",2*1024,NULL,2,0);
    xTaskCreate(task_SendEffekt,"SendEffekt",1*1024,&sendEffectInfo,1,0);
    xTaskCreate(task_updateBattery,"updateBattery",1*1024,&battery,1,0);
    //xTaskCreate(task_gotoDeepSleep,"deepsleep",1*1024,NULL,1,0);
    #ifdef DEBUG_MODE
    xTaskCreate(printSystemInfo,"printSystemInfo",1*1024,&systemInformation,1,0);
    #endif
    
    //start the scheduler
    vTaskStartScheduler(); //Blocking call, to execute code beyond this line it has to be in the form of a Task or interrupt
    while(1);
}

/* [] END OF FILE */
