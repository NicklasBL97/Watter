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

typedef enum CONNECTIONSTATE {
    NOT_CONNECTED = 0,
    CONNECTED = 1
}CONNECTIONSTATE;

typedef struct SendEffekt_t {
    uint16 flags;
    int16 power;
    int16 cadance;
}SendEffekt_t;

cy_stc_ble_conn_handle_t appConnHandle;
CONNECTIONSTATE connState;

void genericEventHandler(uint32 event, void* eventParameter);
void bleInterruptNotify();
void bleTask(void* arg);
void setConnectionHandle(cy_stc_ble_conn_handle_t* handle, void* eventParam);
void SendEffekt(void* arg);
void updateBattery(void* arg);