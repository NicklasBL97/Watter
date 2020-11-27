#pragma once
#include "project.h"

#define LED_ON 0
#define LED_OFF 1

uint8 CPSNotificationsOn(const cy_stc_ble_conn_handle_t connection, const  uint16 cccd);

void handleError(uint8 x);