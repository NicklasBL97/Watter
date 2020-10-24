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

typedef struct SendEffekt_t {
    uint16 flags;
    int16 power;
}SendEffekt_t;


int16 currentPower;
int16 currentCadance;

cy_stc_ble_conn_handle_t appConnHandle;

void genericEventHandler(uint32 event, void* eventParameter);
void bleInterruptNotify();
void bleTask(void* arg);
void setConnectionHandle(cy_stc_ble_conn_handle_t* handle, void* eventParam);
void SendEffekt(void* arg);