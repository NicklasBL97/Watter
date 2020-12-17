#include "project.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "limits.h"
#include "bleHandler.h"
#include "watUtility.h"
#include "Battery.h"

#define CPS_CP_RESP_LENGTH                          (0u)
#define CPS_CP_RESP_OP_CODES                        (1u)
#define CPS_CP_RESP_REQUEST_OP_CODE                 (2u)
#define CPS_CP_RESP_VALUE                           (3u)
#define CPS_CP_RESP_PARAMETER                       (4u)

static cy_stc_ble_conn_handle_t appConnHandle;
static SemaphoreHandle_t bleSemaphore;
SemaphoreHandle_t powerMutex;
CONNECTIONSTATE connState = NOT_CONNECTED;
SendEffekt_t sendEffectInfo;


void handleCY_BLE_SETT_TRANSFERDELAY_CHAR_HANDLE(cy_stc_ble_gatts_write_cmd_req_param_t* parameter){
    uint8 val = parameter->handleValPair.value.val[0];
    if(val == 1 || val == 2 || val == 3 || val == 4)
    {
        updateSettingsGatt(TRANSFERRATE ,val, CY_BLE_GATT_DB_PEER_INITIATED);
        sendEffectInfo.delay = (uint16)(1000/val);
    }
}

void handleCY_BLE_SETT_SAMPLEDELAY_CHAR_HANDLE(cy_stc_ble_gatts_write_cmd_req_param_t* parameter){
    uint8 val = parameter->handleValPair.value.val[0];
    if(val == 5 || val == 10 || val == 15 || val == 20)
    {
        updateSettingsGatt(SAMPLERATE ,val, CY_BLE_GATT_DB_PEER_INITIATED);
        //TODO Write to the parameter to change sample frequency locally
    }
}

void handleGATTS_WRITE_REQ(void* parameter){
    cy_stc_ble_gatts_write_cmd_req_param_t* writeReqParameter = (cy_stc_ble_gatts_write_cmd_req_param_t*)parameter;
    switch(writeReqParameter->handleValPair.attrHandle)
    {
        case CY_BLE_SETT_TRANSFERDELAY_CHAR_HANDLE :
            handleCY_BLE_SETT_TRANSFERDELAY_CHAR_HANDLE(writeReqParameter);
        break;
        
        case CY_BLE_SETT_SAMPLEDELAY_CHAR_HANDLE :
            handleCY_BLE_SETT_SAMPLEDELAY_CHAR_HANDLE(writeReqParameter);
        break;
        
        default :
        
        break;
    }
    Cy_BLE_GATTS_WriteRsp(writeReqParameter->connHandle);
}

///Main BLE event handler, this gets called when an BLE event occurs
void genericEventHandler(uint32 event, void* eventParameter){
    switch (event)
    {
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            LOG("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            Cy_GPIO_Write(LED_CONN_PORT,LED_CONN_NUM,LED_OFF);
            connState = NOT_CONNECTED;
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST,CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
        break;
            
        case CY_BLE_EVT_GATT_CONNECT_IND:
            LOG("CY_BLE_EVT_GATT_CONNECT_IND\r\n");
            
            setConnectionHandle(&appConnHandle,eventParameter);
            connState = CONNECTED;
            Cy_GPIO_Write(LED_CONN_PORT,LED_CONN_NUM,LED_ON);
        break;
        
        case CY_BLE_EVT_GATTS_WRITE_REQ:
            handleGATTS_WRITE_REQ(eventParameter);
        break;
            
        case CY_BLE_EVT_GATTS_INDICATION_ENABLED:
            LOG("CY_BLE_EVT_GATTS_INDICATION_ENABLED\n\r");
        break;
            
        case CY_BLE_EVT_GAP_AUTH_REQ:
            LOG("CY_BLE_EVT_GAP_AUTH_REQ\r\n");
        break;
            
        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            LOG("CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ: handle: %x \r\n", ((cy_stc_ble_gatts_char_val_read_req_t *)eventParameter)->attrHandle);
        break;
        
        case CY_BLE_EVT_CPSS_WRITE_CHAR:
            LOG("CY_BLE_EVT_CPSS_WRITE_CHAR\r\n");
        break;
        
        case CY_BLE_EVT_CPSS_NOTIFICATION_ENABLED:
            LOG("CY_BLE_EVT_CPSS_NOTIFICATION_ENABLED\n\r");
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

        if(CPSNotificationsOn(appConnHandle,&cccd))
        {
            //gather data to be sent
            SendEffekt_t e;
            e.power = sendEffectInfo.power;
            e.cadance = sendEffectInfo.cadance;
            e.flags = sendEffectInfo.flags;
            e.flags |= CY_BLE_CPS_CPM_CRANK_BIT;
            uint8_t powerMeasureData[CY_BLE_GATT_DEFAULT_MTU - 3];
            uint8_t length = 0;
            
            //add data to uint8 array
            addDataToArray(powerMeasureData, &length, e.flags, sizeof(e.flags));
            addDataToArray(powerMeasureData, &length, e.power, sizeof(e.power));
            addDataToArray(powerMeasureData, &length, e.cadance, sizeof(e.cadance));
            addDataToArray(powerMeasureData, &length, 0, sizeof(uint16));
            
            //send the array
            SendEffektNotification(appConnHandle, length, powerMeasureData);

        }
        //wait one period to next transmission
        vTaskDelay(sendEffectInfo.delay);
    }
}

/// task to update the GATT database periodicly with new battery level.
void task_updateBattery(void* arg){
    Battery_t* battery = (Battery_t*)arg;
    
    while(1){
        
        //TODO: add a lookup for which Voltage intervals correspond to different
        //battery percentages in stead of the voltage to percentage conversion here.
        battery->batterylvl = (uint8) 100 * battery->BatteryVoltage/3300.0;
        
        if(battery->batterylvl > 100)
            battery->batterylvl = 100;
        
        Cy_BLE_BASS_SetCharacteristicValue(0,CY_BLE_BAS_BATTERY_LEVEL,sizeof(uint8),&battery->batterylvl);
        vTaskDelay(10000);
    }
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
    Hvp.value.actualLen = 1;
    Hvp.value.len = 1;
    
    if (flags == CY_BLE_GATT_DB_PEER_INITIATED)
    {
        Cy_BLE_GATTS_WriteAttributeValuePeer(&appConnHandle,&Hvp);
    } else {
        Cy_BLE_GATTS_WriteAttributeValueLocal(&Hvp);
        Cy_BLE_GATTS_SendNotification(&appConnHandle,&Hvp);
    }
    
}

