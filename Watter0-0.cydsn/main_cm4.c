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

#define ARM_MATH_CM4
#include "arm_math.h"

#define DEBUG_MODE

#include "bleHandler.h"
#include "printer.h"
#include "sampler.h"

int16 dst;
uint8 batterylvl = 100;
SystemInfo_t systemInformation;

#define SHIFT (16-3)
int16 b0 = 1 * (1 << SHIFT);
int16 b1 = 2 * (1 << SHIFT);
int16 b2 = 1 * (1 << SHIFT);

int16 a1 = -1.142980 * (1 << SHIFT);
int16 a2 =  0.412802 * (1 << SHIFT);

int16 x0 = 0, x1 = 0, x2 = 0, y_1 = 0, y2 = 0;

int16 filOut = 0;
void ADC_ISR_Callback(void){
    x0 = dst << 4;
    int32 yo = (b0 * x0) + (b1 * x1) + (b2 * x2) - (y_1 * a1) - (y2 * a2);
    x2 = x1;
    x1 = x0;
    y2 = y_1;
    y_1 = (yo >> (SHIFT));
    filOut = y_1;
    Cy_DMA_Channel_ClearInterrupt(DMA_Sample_HW, 0);
}


int main(void)
{
    //DMA Setup
//    cy_stc_dma_channel_config_t channelConfig =
//    {
//        .descriptor = &DMA_Sample_Descriptor_1,
//        .preemptable = DMA_Sample_PREEMPTABLE,
//        .priority = DMA_Sample_PRIORITY,
//        .enable = false
//    };
//    (void)Cy_DMA_Descriptor_Init(&DMA_Sample_Descriptor_1,
//    &DMA_Sample_Descriptor_1_config);
//    (void)Cy_DMA_Channel_Init(DMA_Sample_HW, DMA_Sample_DW_CHANNEL, &channelConfig);
//    Cy_DMA_Descriptor_SetSrcAddress(&DMA_Sample_Descriptor_1, (uint32_t *) &(SAR->CHAN_RESULT[0]));
//    Cy_DMA_Descriptor_SetDstAddress(&DMA_Sample_Descriptor_1, &dst);
//    Cy_DMA_Channel_Enable(DMA_Sample_HW, DMA_Sample_DW_CHANNEL);
//    Cy_DMA_Enable(DMA_Sample_HW);

    DMA_Sample_Start((uint32_t *) &(SAR->CHAN_RESULT[0]),&dst);
    Cy_DMA_Channel_SetInterruptMask(DMA_Sample_HW,0,CY_DMA_INTR_MASK);
    SendEffekt_init();
    sampler_init();
    printer_init();
    
    systemInformation.effekt = &y_1;
    systemInformation.cadance = &sendEffectInfo.cadance;
    systemInformation.batterylvl = &batterylvl;
    
    __enable_irq();
    Cy_SysInt_Init(&Sample_Int_cfg,ADC_ISR_Callback); 
    NVIC_ClearPendingIRQ(Sample_Int_cfg.intrSrc);
	NVIC_EnableIRQ(Sample_Int_cfg.intrSrc);
    
    ADC_Start();
    //ADC_IRQ_Enable();
    ADC_StartConvert();
    
    UART_Start();
    
    setvbuf(stdin,NULL,_IONBF,0);
    setvbuf(stdout,NULL,_IONBF,0);
    
    xTaskCreate(task_ble,"bleTask",2*1024,NULL,2,0);
    xTaskCreate(task_SendEffekt,"SendEffekt",1*1024,&sendEffectInfo,1,0);
    xTaskCreate(task_updateBattery,"updateBattery",1*1024,&batterylvl,1,0);
    xTaskCreate(task_sampler,"sampler",1*1024,&samples,1,0);
    #ifdef DEBUG_MODE
    xTaskCreate(printSystemInfo,"printSystemInfo",1*1024,&systemInformation,1,0);
    #endif
    vTaskStartScheduler(); //Blocking call, to execute code beyond this line it has to be in the form of a Task
    while(1){}
}

/* [] END OF FILE */
