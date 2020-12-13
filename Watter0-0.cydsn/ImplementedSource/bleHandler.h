/**
 * @file bleHandler.h
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
#include "FreeRTOS.h"
#include "semphr.h"
#include "printer.h"

/**
 * @brief 
 * 
 */
typedef enum CONNECTIONSTATE {
    NOT_CONNECTED = 0,
    CONNECTED = 1
}CONNECTIONSTATE;

/**
 * @brief 
 * 
 */
typedef struct SendEffekt_t {
    uint16 flags;
    int16 power;
    int16 cadance;
    uint16 delay;
}SendEffekt_t;

/**
 * @brief 
 * 
 */
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

/**
 * @brief 
 * 
 * @param arg 
 */
void task_ble(void* arg);



/**
 * @brief 
 * 
 * @param arg 
 */
void task_updateBattery(void* arg);

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
 */
void SendEffekt_init();

/**
 * @brief 
 * 
 */
void sampler_init();

/**
 * @brief 
 * 
 * @param setting 
 * @param value 
 * @param flags 
 */
void updateSettingsGatt(settings_t setting ,uint16 value, uint8 flags);