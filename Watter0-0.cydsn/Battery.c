#include "Battery.h"

void BatteryInit(Battery_t* battery){
    battery->BatteryVoltage = 0;
    battery->batterylvl = 0;
}