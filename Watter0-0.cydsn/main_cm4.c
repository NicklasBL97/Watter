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
#include "my_I2C.h"
#include "ADXL345Sensor.h"

#define ARM_MATH_CM4
#include "arm_math.h"

//#define DEBUG_MODE

#include "bleHandler.h"
#include "printer.h"

uint32 dst[2];
uint8 batterylvl = 0;
SystemInfo_t systemInformation;
ADXL345Data accelerometerData;


void ADC_ISR_Callback(void){

    sendEffectInfo.power = ADC_CountsTo_mVolts(0,dst[0]);
    batterylvl = (uint8) (100 * dst[1]/((float)(1<<12)));
    Cy_DMA_Channel_ClearInterrupt(DMA_Sample_HW, 0);
}

typedef struct AccelerometerData{
    int16 x;
    int16 y;
    int16 z;
}AccelerometerData;

//uint32 Timer_Count2ms(uint32 count, uint32 fclk){
//    return 1000 * (float)count/(float)fclk;
//}

void Cad_ISR_Callback(void){
    accelerometerData = ADXL345GetData();
}

int main(void)
{
    DMA_Sample_Start((uint32_t *) &(SAR->CHAN_RESULT[0]),&dst[0]);
    Cy_DMA_Channel_SetInterruptMask(DMA_Sample_HW,0,CY_DMA_INTR_MASK);
    
    SendEffekt_init();
    //sampler_init();
    printer_init();
    
    I2C_Start();
    ADXL345Init();
    
    systemInformation.effekt = &sendEffectInfo.power;
    systemInformation.cadance = &sendEffectInfo.cadance;
    systemInformation.batterylvl = &batterylvl;
    systemInformation.accData = &accelerometerData;
    
    __enable_irq();
    Cy_SysInt_Init(&Sample_Int_cfg,ADC_ISR_Callback); 
    NVIC_ClearPendingIRQ(Sample_Int_cfg.intrSrc);
	NVIC_EnableIRQ(Sample_Int_cfg.intrSrc);
    
    Cy_SysInt_Init(&Cad_int_cfg,Cad_ISR_Callback); 
    NVIC_ClearPendingIRQ(Cad_int_cfg.intrSrc);
	NVIC_EnableIRQ(Cad_int_cfg.intrSrc);
    
    ADC_Start();
    //ADC_IRQ_Enable();
    ADC_StartConvert();
    UART_Start();
    

    
    Cy_TCPWM_Counter_Init(Timer_Cad_Sample_HW, Timer_Cad_Sample_CNT_NUM,&Timer_Cad_Sample_config);
    Cy_TCPWM_Enable_Multiple(Timer_Cad_Sample_HW, Timer_Cad_Sample_CNT_MASK);
    Cy_TCPWM_TriggerStart(Timer_Cad_Sample_HW,Timer_Cad_Sample_CNT_MASK);
    
    
    setvbuf(stdin,NULL,_IONBF,0);
    setvbuf(stdout,NULL,_IONBF,0);
    
    xTaskCreate(task_ble,"bleTask",2*1024,NULL,2,0);
    xTaskCreate(task_SendEffekt,"SendEffekt",1*1024,&sendEffectInfo,1,0);
    xTaskCreate(task_updateBattery,"updateBattery",1*1024,&batterylvl,1,0);
    #ifdef DEBUG_MODE
    xTaskCreate(printSystemInfo,"printSystemInfo",1*1024,&systemInformation,1,0);
    #endif
    vTaskStartScheduler(); //Blocking call, to execute code beyond this line it has to be in the form of a Task
    while(1){}
}

/* [] END OF FILE */
