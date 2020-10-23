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

cy_stc_ble_conn_handle_t appConnHandle;

void genericEventHandler(uint32 event, void* eventParameter);
void bleInterruptNotify();
void bleTask(void* arg);
void setConnectionHandle(cy_stc_ble_conn_handle_t* handle, void* eventParam);
void SendEffekt(void* arg);