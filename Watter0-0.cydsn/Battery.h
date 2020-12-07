#pragma once
#include "project.h"

typedef struct {
    uint16 BatteryVoltage;
    uint8 batterylvl;
}Battery_t;

void BatteryInit(Battery_t* battery);