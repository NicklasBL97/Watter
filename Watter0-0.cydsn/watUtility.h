#pragma once
#include "project.h"

#define LED_ON 0
#define LED_OFF 1

uint8 CPSNotificationsOn(const cy_stc_ble_conn_handle_t connection, const  uint16 cccd);
void addDataToArray(uint8_t * array, uint8* stride, uint16 data, uint8 sizeOfData);
void SendEffektNotification(cy_stc_ble_conn_handle_t connection, uint8 stride, uint8_t * array);
void setConnectionHandle(cy_stc_ble_conn_handle_t* handle, void* eventParam);
void handleError(uint8 x);