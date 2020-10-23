
#include "project.h"
#include "stdio.h"
int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    Cy_BLE_Start(0);
    
    /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 

    UART_Start();
    //ADC_1_Start();
    //ADC_1_StartConvert();
    //VDAC_1_Start();
    //uint16 counter = 0;
    //uint16 result;
    
    //printf("Starting program\n\r");
    for(;;)
    {
        Cy_BLE_ProcessEvents();
    }
}

