#include "watUtility.h"
#include "printer.h"

///Utility function to check if the current connection has enabled CPS notifications
uint8 CPSNotificationsOn(cy_stc_ble_conn_handle_t connection, const uint16_t* cccd){
    
    
    Cy_BLE_CPSS_GetCharacteristicDescriptor(connection, CY_BLE_CPS_POWER_MEASURE, CY_BLE_CPS_CCCD, CY_BLE_CCCD_LEN, (uint8*)cccd);
          
    if(*cccd == CY_BLE_CCCD_NOTIFICATION)
        return 1;

    return 0;
}

///Utility function to add one data element to an array that can be send with the function SendEffektNotification
void addDataToArray(uint8_t * array, uint8* stride, uint16 data, uint8 sizeOfData){
    Cy_BLE_Set16ByPtr(array + *stride, data);
    *stride += sizeOfData;
}

///Error handler function that prints to the debugging interface and turns on the error indicating LED
void handleError(uint8 x)
{
    Cy_GPIO_Write(LED_ERROR_PORT,LED_ERROR_NUM,LED_ON);
    LOG(" error %d \r\n ", x);
}

///Utility function to do everything involved in transmitting the CPSS notification
void SendEffektNotification(cy_stc_ble_conn_handle_t connection, uint8 stride, uint8_t * array){
    do
        {
        Cy_BLE_ProcessEvents();
        }
        while(Cy_BLE_GATT_GetBusyStatus(connection.attId) == CY_BLE_STACK_STATE_BUSY);
        
        
        if(Cy_BLE_GetConnectionState(connection) >= CY_BLE_CONN_STATE_CONNECTED)
        {
            Cy_BLE_CPSS_SendNotification(connection, CY_BLE_CPS_POWER_MEASURE, stride, array);
        }
}

void setConnectionHandle(cy_stc_ble_conn_handle_t* handle, void* eventParam){
    *handle = *(cy_stc_ble_conn_handle_t *)eventParam;
}