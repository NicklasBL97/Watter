/**
 * @file watUtility.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#include "project.h"

#define LED_ON 0
#define LED_OFF 1

/**
 * @brief Check if notifications on the CPS is on
 * 
 * @param connection The current connection handle
 * @param cccd The place to store Client Characteristic Configuration Descriptor
 * @return uint8 0 if off and 1 if on
 */
uint8 CPSNotificationsOn(const cy_stc_ble_conn_handle_t connection, const  uint16_t* cccd);

/**
 * @brief Function to add data in a byte
 * 
 * @param array 
 * @param stride 
 * @param data 
 * @param sizeOfData 
 */
void addDataToArray(uint8_t * array, uint8* stride, uint16 data, uint8 sizeOfData);

/**
 * @brief 
 * 
 * @param connection 
 * @param stride 
 * @param array 
 */
void SendEffektNotification(cy_stc_ble_conn_handle_t connection, uint8 stride, uint8_t * array);

/**
 * @brief Set the Connection Handle object
 * 
 * @param handle 
 * @param eventParam 
 */
void setConnectionHandle(cy_stc_ble_conn_handle_t* handle, void* eventParam);

/**
 * @brief 
 * 
 * @param x 
 */
void handleError(uint8 x);