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
#pragma once
#include "project.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "printer.h"

typedef enum CONNECTIONSTATE {
    NOT_CONNECTED = 0,
    CONNECTED = 1
}CONNECTIONSTATE;

typedef struct SendEffekt_t {
    uint16 flags;
    int16 power;
    int16 cadance;
    uint16 delay;
}SendEffekt_t;

typedef enum settings_t{
    TRANSFERRATE,
    SAMPLERATE
}settings_t;

/**
 * @brief Enum for different states of connection to a client
 * 
 */
extern CONNECTIONSTATE connState;

/**
 * @brief the struct used for any information to be send with the function \ref <task_SendEffekt>"()"
 * 
 */
extern SendEffekt_t sendEffectInfo;

/**
 * @brief The mutex locked when using \p sendEffectInfo
 * 
 */
extern SemaphoreHandle_t powerMutex;

/**
 * @brief the FreeRTOS task to send CPS notifications
 * 
 * @param arg 
 */
void task_SendEffekt(void* arg);

/***************************************************************************
Function to process BLE events.

This function processes BLE events when possible. events are usually created by the user from the client.
The function is implemented as a FreeTROS task, and should be added to scheduler using xTaskCreate().
 @param arg Unused.
 @return void.
****************************************************************************/
void task_ble(void* arg);



/***************************************************************************
Function to sample battery level.

This function periodically samples a battery sensor.
The function is implemented as a FreeTROS task, and should be added to scheduler using xTaskCreate().
 @param arg void pointer to the datastructure where sampled values is placed.
 @return void.
****************************************************************************/
void task_updateBattery(void* arg);

void setConnectionHandle(cy_stc_ble_conn_handle_t* handle, void* eventParam);

void SendEffekt_init();
void sampler_init();

void updateSettingsGatt(settings_t setting ,uint16 value, uint8 flags);