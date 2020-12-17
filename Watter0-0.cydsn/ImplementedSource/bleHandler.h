/**
 * @file bleHandler.h
 * @author Nicklas Lind
 * @brief This header is included to add all BLE functionallity
 * @version 0.1
 * @date 2020-12-13
 * 
 * 
 * 
 */
#pragma once
#include "project.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "printer.h"

/**
 * @brief The enum for the differnt states of BLE connection
 * 
 */
typedef enum CONNECTIONSTATE {
    NOT_CONNECTED = 0,
    CONNECTED = 1
}CONNECTIONSTATE;

/**
 * @brief The struct to store neccessary information when sending CPS notifications
 * 
 */
typedef struct SendEffekt_t {
    uint16 flags;
    int16 power;
    int16 cadance;
    uint16 delay;
}SendEffekt_t;

/**
 * @brief The enum for different possible settings to change
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
 * @brief The struct used for any information to be send with a CPS notification
 * 
 */
extern SendEffekt_t sendEffectInfo;

/**
 * @brief The mutex locked when using \p sendEffectInfo
 * 
 */
extern SemaphoreHandle_t powerMutex;

/**
 * @brief The FreeRTOS task to send CPS notifications
 * 
 * @param arg unused
 */
void task_SendEffekt(void* arg);

/**
 * @brief The task called when a BLE event occurs
 * This task calls the Cy_BLE_ProcessEvents() which will call the callback function
 * 
 * @param arg unused
 */
void task_ble(void* arg);



/**
 * @brief This task periodically updates the battery level in the GATT database.
 * The GATT database is only updated at a certain frequency way below the sampling rate
 * to avoid overloading the system by updating the database at a high frequency
 * 
 * @param arg A pointer to the used Battery_t struct
 */
void task_updateBattery(void* arg);

/**
 * @brief helper function to set the Connection Handle object
 * 
 * @param handle The handle to be updated
 * @param eventParam The event parameter from the event handler
 */
void setConnectionHandle(cy_stc_ble_conn_handle_t* handle, void* eventParam);

/**
 * @brief Init function for the sendEffectInfo variable
 * 
 */
void SendEffekt_init();

/**
 * @brief Helper function for updating the GATT database and settings_t struct with new settings
 * 
 * @param setting The setting to update 
 * @param value The new value for the setting
 * @param flags indicator if the change is peer initiated or not
 */
void updateSettingsGatt(settings_t setting ,uint16 value, uint8 flags);