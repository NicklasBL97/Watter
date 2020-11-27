#include "watUtility.h"
#include "printer.h"
uint8 CPSNotificationsOn(cy_stc_ble_conn_handle_t connection, uint16 cccd){
    
    
    Cy_BLE_CPSS_GetCharacteristicDescriptor(connection, CY_BLE_CPS_POWER_MEASURE, CY_BLE_CPS_CCCD, CY_BLE_CCCD_LEN, (uint8_t*)&cccd);
          
    if(cccd == CY_BLE_CCCD_NOTIFICATION)
        return 1;

    return 0;
}

void addDataToArray(uint8_t * array, uint8 stride, uint8 data, uint8 sizeOfData){
    Cy_BLE_Set16ByPtr(array, data);
    stride += sizeOfData;
}

void handleError(uint8 x)
{
    Cy_GPIO_Write(LED_ERROR_PORT,LED_ERROR_NUM,LED_ON);
    LOG(" error %d \r\n ", x);
}