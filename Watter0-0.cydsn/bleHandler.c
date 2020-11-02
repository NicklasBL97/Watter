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
#include "sampler.h"

#define LED_ON 0
#define LED_OFF 1

#define CPS_CP_RESP_LENGTH                          (0u)
#define CPS_CP_RESP_OP_CODES                        (1u)
#define CPS_CP_RESP_REQUEST_OP_CODE                 (2u)
#define CPS_CP_RESP_VALUE                           (3u)
#define CPS_CP_RESP_PARAMETER                       (4u)


static cy_stc_ble_conn_handle_t appConnHandle;
static SemaphoreHandle_t bleSemaphore;
CONNECTIONSTATE connState = NOT_CONNECTED;

SendEffekt_t sendEffectInfo;
SemaphoreHandle_t powerMutex;

void genericEventHandler(uint32 event, void* eventParameter){
    //printf("Event: 0x%x\n\r",event);
    switch (event)
    {
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            xSemaphoreTake(printerSema,portMAX_DELAY);
            printf("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            xSemaphoreGive(printerSema);
            Cy_GPIO_Write(LED_ADV_PORT,LED_ADV_NUM,LED_ON);
            Cy_GPIO_Write(LED_CONN_PORT,LED_CONN_NUM,LED_OFF);
            connState = NOT_CONNECTED;
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST,CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
        break;
            
        case CY_BLE_EVT_GATT_CONNECT_IND:
            xSemaphoreTake(printerSema,portMAX_DELAY);
            printf("CY_BLE_EVT_GATT_CONNECT_IND\r\n");
            xSemaphoreGive(printerSema);
            
            setConnectionHandle(&appConnHandle,eventParameter);
            connState = CONNECTED;
            Cy_GPIO_Write(LED_CONN_PORT,LED_CONN_NUM,LED_ON);
            Cy_GPIO_Write(LED_ADV_PORT,LED_ADV_NUM,LED_OFF);
        break;
        
        case CY_BLE_EVT_GATTS_WRITE_REQ:
            {
                cy_stc_ble_gatts_write_cmd_req_param_t* writeReqParameter = (cy_stc_ble_gatts_write_cmd_req_param_t*)eventParameter;
                
                if (writeReqParameter->handleValPair.attrHandle == CY_BLE_SETT_TRANSFERDELAY_CHAR_HANDLE) {
                    uint8 val = writeReqParameter->handleValPair.value.val[0];
                    if(val == 1 || val == 2 || val == 3 || val == 4)
                    {
                        updateSettingsGatt(TRANSFERRATE ,val, CY_BLE_GATT_DB_PEER_INITIATED);
                        sendEffectInfo.delay = (uint16)(1000/val);
                    }
                }
                if(writeReqParameter->handleValPair.attrHandle == CY_BLE_SETT_SAMPLEDELAY_CHAR_HANDLE){
                    uint8 val = writeReqParameter->handleValPair.value.val[0];
                    if(val == 5 || val == 10 || val == 15 || val == 20)
                    {
                        updateSettingsGatt(SAMPLERATE ,val, CY_BLE_GATT_DB_PEER_INITIATED);
                        samples.delay = (uint16)(1000/val);
                    }
                }
            Cy_BLE_GATTS_WriteRsp(writeReqParameter->connHandle);
            }
        break;
            
        case CY_BLE_EVT_GATTS_INDICATION_ENABLED:
            xSemaphoreTake(printerSema,portMAX_DELAY);
            printf("CY_BLE_EVT_GATTS_INDICATION_ENABLED\n\r");
            xSemaphoreGive(printerSema);
            
        break;
            
        case CY_BLE_EVT_GAP_AUTH_REQ:
            //printf("CY_BLE_EVT_GAP_AUTH_REQ\r\n");
        break;
            
        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            xSemaphoreTake(printerSema,portMAX_DELAY);
            printf("CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ: handle: %x \r\n", ((cy_stc_ble_gatts_char_val_read_req_t *)eventParameter)->attrHandle);
            xSemaphoreGive(printerSema);
            
        break;
        
        case CY_BLE_EVT_CPSS_WRITE_CHAR:
            //printf("CY_BLE_EVT_CPSS_WRITE_CHAR\r\n");
        break;
        
        case CY_BLE_EVT_CPSS_NOTIFICATION_ENABLED:
            xSemaphoreTake(printerSema,portMAX_DELAY);
            printf("CY_BLE_EVT_CPSS_NOTIFICATION_ENABLED\n\r");
            xSemaphoreGive(printerSema);
            //printf("CY_BLE_EVT_CPSS_NOTIFICATION_ENABLED: char: %x\r\n", locCharIndex);
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

void task_ble(void* arg){
    (void)arg;
    
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

void task_SendEffekt(void* arg){
    
    while(1){
        uint16_t cccd = CY_BLE_CCCD_DEFAULT;
        Cy_BLE_CPSS_GetCharacteristicDescriptor(appConnHandle, CY_BLE_CPS_POWER_MEASURE, CY_BLE_CPS_CCCD, CY_BLE_CCCD_LEN, (uint8_t*)&cccd);
          
        if(cccd == CY_BLE_CCCD_NOTIFICATION)
        {
            
            SendEffekt_t e;
            e.power = sendEffectInfo.power;
            e.cadance = sendEffectInfo.cadance;
            e.flags = sendEffectInfo.flags;
            e.flags |= CY_BLE_CPS_CPM_CRANK_BIT;
            uint8_t powerMeasureData[CY_BLE_GATT_DEFAULT_MTU - 3];
            uint8_t length = 0;
        
            Cy_BLE_Set16ByPtr(powerMeasureData, e.flags);
            length += sizeof(e.flags);
            Cy_BLE_Set16ByPtr(powerMeasureData + length, e.power);
            length += sizeof(e.power);
            Cy_BLE_Set16ByPtr(powerMeasureData + length, e.cadance);
            length += sizeof(e.cadance);
            Cy_BLE_Set16ByPtr(powerMeasureData + length, 0);
            length += sizeof(e.cadance);
            
            do
            {
            Cy_BLE_ProcessEvents();
            }
            while(Cy_BLE_GATT_GetBusyStatus(appConnHandle.attId) == CY_BLE_STACK_STATE_BUSY);
            
            
            if(Cy_BLE_GetConnectionState(appConnHandle) >= CY_BLE_CONN_STATE_CONNECTED)
            {
                Cy_BLE_CPSS_SendNotification(appConnHandle, CY_BLE_CPS_POWER_MEASURE, length, powerMeasureData);
            }
        }
        vTaskDelay(sendEffectInfo.delay);
    }
    
}

void task_updateBattery(void* arg){
    uint8* batterylvl = arg;
    while(1){
        (*batterylvl)--;
        
        if(*batterylvl > 100)
            *batterylvl = 100;
        
        Cy_BLE_BASS_SetCharacteristicValue(0,CY_BLE_BAS_BATTERY_LEVEL,sizeof(uint8),batterylvl);
        
        vTaskDelay(10000);
    }
}

void setConnectionHandle(cy_stc_ble_conn_handle_t* handle, void* eventParam){
    *handle = *(cy_stc_ble_conn_handle_t *)eventParam;
}

void SendEffekt_init(){
    sendEffectInfo.power = 0;
    sendEffectInfo.cadance = 0;
    sendEffectInfo.flags = 0;
    sendEffectInfo.delay = 250;
}

void updateSettingsGatt(settings_t setting ,uint16 value, uint8 flags){
    cy_stc_ble_gatt_handle_value_pair_t Hvp;
    
    switch(setting)
    {
        case TRANSFERRATE: 
            Hvp.attrHandle = CY_BLE_SETT_TRANSFERDELAY_CHAR_HANDLE;
        break;
        
        case SAMPLERATE:
            Hvp.attrHandle = CY_BLE_SETT_SAMPLEDELAY_CHAR_HANDLE;
        break;
    }
    
    Hvp.value.val = (uint8*)&value;
    Hvp.value.actualLen = 2;
    Hvp.value.len = 2;
    
    if (flags == CY_BLE_GATT_DB_PEER_INITIATED)
    {
        Cy_BLE_GATTS_WriteAttributeValuePeer(&appConnHandle,&Hvp);
    } else {
        Cy_BLE_GATTS_WriteAttributeValueLocal(&Hvp);
        Cy_BLE_GATTS_SendNotification(&appConnHandle,&Hvp);
    }
    
}

