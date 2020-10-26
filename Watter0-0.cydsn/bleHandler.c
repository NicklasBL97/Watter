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
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "limits.h"
#include "bleHandler.h"

#define LED_ON 0
#define LED_OFF 1

#define CPS_CP_RESP_LENGTH                          (0u)
#define CPS_CP_RESP_OP_CODES                        (1u)
#define CPS_CP_RESP_REQUEST_OP_CODE                 (2u)
#define CPS_CP_RESP_VALUE                           (3u)
#define CPS_CP_RESP_PARAMETER                       (4u)



cy_stc_ble_conn_handle_t appConnHandle;
static SemaphoreHandle_t bleSemaphore;
CONNECTIONSTATE connState = NOT_CONNECTED;

static uint8_t powerCPData[CY_BLE_GATT_DEFAULT_MTU - 2u] = {3u, CY_BLE_CPS_CP_OC_RC, CY_BLE_CPS_CP_OC_SCV, CY_BLE_CPS_CP_RC_SUCCESS};

void genericEventHandler(uint32 event, void* eventParameter)
{
    void* parameter = eventParameter;
    //setting a char_value
    //uint8 batterylvl = 42;
    //Cy_BLE_BASS_SetCharacteristicValue(0,CY_BLE_BAS_BATTERY_LEVEL,sizeof(uint8),&batterylvl);
    //cy_stc_ble_gatts_char_val_read_req_t* readReqParameter;
    
    uint8_t locCharIndex;
    locCharIndex = ((cy_stc_ble_cps_char_value_t *)eventParameter)->charIndex;
    uint32_t i;
    //printf("Event: 0x%x\n\r",event);
    switch (event)
    {
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            //printf("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            Cy_GPIO_Write(LED_ADV_PORT,LED_ADV_NUM,LED_ON);
            Cy_GPIO_Write(LED_CONN_PORT,LED_CONN_NUM,LED_OFF);
            connState = NOT_CONNECTED;
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST,CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
        break;
            
        case CY_BLE_EVT_GATT_CONNECT_IND:
            //printf("CY_BLE_EVT_GATT_CONNECT_IND\r\n");
            setConnectionHandle(&appConnHandle,eventParameter);
            connState = CONNECTED;
            Cy_GPIO_Write(LED_CONN_PORT,LED_CONN_NUM,LED_ON);
            Cy_GPIO_Write(LED_ADV_PORT,LED_ADV_NUM,LED_OFF);
        break;
        
        case CY_BLE_EVT_GATTS_WRITE_REQ:
            //printf("CY_BLE_EVT_GATTS_WRITE_REQ attr handle: %4.4x , value: ",((cy_stc_ble_gatts_write_cmd_req_param_t *)eventParameter)->handleValPair.attrHandle);
            for(i = 0; i < ((cy_stc_ble_gatts_write_cmd_req_param_t *)eventParameter)->handleValPair.value.len; i++)
            {
                //printf("%2.2x ", ((cy_stc_ble_gatts_write_cmd_req_param_t *)eventParameter)->handleValPair.value.val[i]);
            }
            //printf("\r\n");
            //Cy_BLE_GATTS_WriteEventHandler((cy_stc_ble_gatts_write_cmd_req_param_t *)eventParameter);
            Cy_BLE_GATTS_WriteRsp(appConnHandle);
        break;
            
        case CY_BLE_EVT_GATTS_INDICATION_ENABLED:
            //printf("CY_BLE_EVT_GATTS_INDICATION_ENABLED\n\r");
        break;
            
        case CY_BLE_EVT_GAP_AUTH_REQ:
            //printf("CY_BLE_EVT_GAP_AUTH_REQ\r\n");
        break;
            
        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            //printf("CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ: handle: %x \r\n", ((cy_stc_ble_gatts_char_val_read_req_t *)eventParameter)->attrHandle);
        break;
        
        case CY_BLE_EVT_CPSS_WRITE_CHAR:
            //printf("CY_BLE_EVT_CPSS_WRITE_CHAR\r\n");
        break;
        
        case CY_BLE_EVT_CPSS_NOTIFICATION_ENABLED:
            //printf("CY_BLE_EVT_CPSS_NOTIFICATION_ENABLED: char: %x\r\n", locCharIndex);
        break;
            
        default:
        break;
    }
    
}

void SendEffekt(void* arg){
    SendEffekt_t* effektinfo = (SendEffekt_t*)arg;
    
    int16* currentPower = &effektinfo->power;
    
    cy_en_ble_api_result_t apiResult = CY_BLE_SUCCESS;
    //printf("SendEffekt task started\r\n");
    
    while(1){
        uint16_t cccd = CY_BLE_CCCD_DEFAULT;
        apiResult = Cy_BLE_CPSS_GetCharacteristicDescriptor(appConnHandle, CY_BLE_CPS_POWER_MEASURE, CY_BLE_CPS_CCCD, CY_BLE_CCCD_LEN, (uint8_t*)&cccd);
          
        if(apiResult != CY_BLE_SUCCESS)
        {
            //printf("Cy_BLE_CPSS_GetCharacteristicDescriptor API Error: 0x%x \r\n", apiResult);
        }
        else if(cccd == CY_BLE_CCCD_NOTIFICATION)
        {
            
            SendEffekt_t e;
            e.power = *currentPower;
            e.flags = 0x0000;

            uint8_t powerMeasureData[CY_BLE_GATT_DEFAULT_MTU - 3];
            uint8_t length = 0;
        
            Cy_BLE_Set16ByPtr(powerMeasureData, (CY_BLE_CPS_CPM_TORQUE_PRESENT_BIT |
                                            CY_BLE_CPS_CPM_TORQUE_SOURCE_BIT |
                                            CY_BLE_CPS_CPM_WHEEL_BIT |
                                            CY_BLE_CPS_CPM_ENERGY_BIT) & e.flags);
        
            length += sizeof(e.flags);
            Cy_BLE_Set16ByPtr(powerMeasureData + length, e.power);
            length += sizeof(e.power);

            //printf("processing events\n\r");
            do
            {
            Cy_BLE_ProcessEvents();
            }
            while(Cy_BLE_GATT_GetBusyStatus(appConnHandle.attId) == CY_BLE_STACK_STATE_BUSY);
            
            
            if(Cy_BLE_GetConnectionState(appConnHandle) >= CY_BLE_CONN_STATE_CONNECTED)
            {
            
            //printf("sending data\n\r");
            apiResult = Cy_BLE_CPSS_SendNotification(appConnHandle, CY_BLE_CPS_POWER_MEASURE, length, powerMeasureData);
            if(apiResult != CY_BLE_SUCCESS)
                {
                    //printf("CpssSendNotification API Error: %x \r\n", apiResult);
                }
            
            }
        }
        vTaskDelay(250);
    }
    
}

void updateBattery(void* arg){
    uint8* batterylvl = arg;
    while(1){
        (*batterylvl)--;
        
        if(*batterylvl < 0)
            *batterylvl = 100;
        
        Cy_BLE_BASS_SetCharacteristicValue(0,CY_BLE_BAS_BATTERY_LEVEL,sizeof(uint8),batterylvl);
        
        vTaskDelay(10000);
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
    
    //printf("BLE task started\r\n");
    
    bleSemaphore = xSemaphoreCreateCounting(UINT_MAX,0);
    
    Cy_BLE_Start(genericEventHandler);
    
    while(Cy_BLE_GetState() != CY_BLE_STATE_ON)
    {
        Cy_BLE_ProcessEvents();   
    }
    
    Cy_BLE_RegisterAppHostCallback(bleInterruptNotify);
    Cy_BLE_CPS_RegisterAttrCallback(bleInterruptNotify);
    Cy_BLE_BAS_RegisterAttrCallback(bleInterruptNotify);
    while(1)
    {
        xSemaphoreTake(bleSemaphore,portMAX_DELAY);
        Cy_BLE_ProcessEvents();
    }
}

void setConnectionHandle(cy_stc_ble_conn_handle_t* handle, void* eventParam){
    *handle = *(cy_stc_ble_conn_handle_t *)eventParam;
}