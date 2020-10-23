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

#define LED_ON 0
#define LED_OFF 1

SemaphoreHandle_t bleSemaphore;

void genericEventHandler(uint32 event, void* eventParameter)
{
    //setting a char_value
    //uint8 batterylvl = 42;
    //Cy_BLE_BASS_SetCharacteristicValue(0,CY_BLE_BAS_BATTERY_LEVEL,sizeof(uint8),&batterylvl);
    
    //cy_stc_ble_gatts_char_val_read_req_t* readReqParameter;
    printf("Event: 0x%x\n\r",event);
    switch (event)
    {
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            printf("Not connected\r\n");
            Cy_GPIO_Write(LED_ADV_PORT,LED_ADV_NUM,LED_ON);
            Cy_GPIO_Write(LED_CONN_PORT,LED_CONN_NUM,LED_OFF);
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST,CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
        break;
            
        case CY_BLE_EVT_GATT_CONNECT_IND:
            printf("Connected\r\n");
            Cy_GPIO_Write(LED_CONN_PORT,LED_CONN_NUM,LED_ON);
            Cy_GPIO_Write(LED_ADV_PORT,LED_ADV_NUM,LED_OFF);
        break;
        
        case CY_BLE_EVT_CSCSC_NOTIFICATION:
            //printf("NOTIFICATION ENABLED\r\n");
        break;
            
        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            //readReqParameter = (cy_stc_ble_gatts_char_val_read_req_t*)eventParameter;

            printf("reading a GATT char_val\r\n");
        break;
            
        default:
        break;
    }
    
}

void bleInterruptNotify(){
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(bleSemaphore,&xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void bleTask(void* arg){
    (void)arg;
    
    printf("BLE task started\r\n");
    
    bleSemaphore = xSemaphoreCreateCounting(UINT_MAX,0);
    
    Cy_BLE_Start(genericEventHandler);
    
    while(Cy_BLE_GetState() != CY_BLE_STATE_ON)
    {
        Cy_BLE_ProcessEvents();   
    }
    
    Cy_BLE_RegisterAppHostCallback(bleInterruptNotify);
    
    while(1)
    {
        xSemaphoreTake(bleSemaphore,portMAX_DELAY);
        Cy_BLE_ProcessEvents();
    }
}

void SendEffekt(void* arg){
    (void)arg;
    printf("SendEffekt task started\r\n");
    
    while(1){
        uint8_t attribute;
//        Cy_BLE_CSCSS_GetCharacteristicValue(CY_BLE_CSCS_CSC_MEASUREMENT,sizeof(uint8_t),(uint8_t*)&attribute);
//        printf("%d\n\r",attribute);
//        attribute += 10;
//        Cy_BLE_CSCSS_SetCharacteristicValue(CY_BLE_CSCS_CSC_MEASUREMENT,sizeof(uint8_t),(uint8_t*)&attribute);
        vTaskDelay(250);
        
        cy_stc_ble_conn_handle_t connhandle;
        Cy_BLE_CSCSS_SendNotification(connhandle,CY_BLE_CSCS_CSC_MEASUREMENT,sizeof(uint8_t),(uint8_t*)&attribute);
    }
}

int main(void)
{
    __enable_irq();
    
    UART_Start();
    
    setvbuf(stdin,NULL,_IONBF,0);
    setvbuf(stdout,NULL,_IONBF,0);
    printf("System Started\n\r");
    
    xTaskCreate(bleTask,"bleTask",4*1024,NULL,1,0);
    xTaskCreate(SendEffekt,"SendEffekt",2*1024,NULL,1,0);
    vTaskStartScheduler(); //Blocking call, to execute code beyond this line it has to be in the form of a Task
}

/* [] END OF FILE */
