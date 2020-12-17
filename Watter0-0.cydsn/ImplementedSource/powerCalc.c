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

#include "ImplementedSource/powerCalc.h"

static uint8 sampleCount = 0;
static uint16 effectSampleValue[arraySize];
static uint8 samplesSinceBroadcast = 0;

uint8 samplesPrBroadcast = FS/BROADCAST_FS;
float timeBetween = ((1/FS)*BROADCAST_FS);   // Time between broadcasts in seconds;
uint8 armLength = 175;          // 175 mm
uint8 calibration = 35;        // mV/kg

uint16 averageVoltage;
uint16 effect;
uint32 voltageAccu = 0;
float cadenceSec = 0;                 
float distance = 0;

static powerState Pstate = START;

void handleSample(uint16 powerVoltage)
{
    effectSampleValue[sampleCount] = powerVoltage;
    
    samplesSinceBroadcast++;
    sampleCount++;
    if (sampleCount == FS-1)
    {
        sampleCount = 0;
    }
}

uint16 getPower(int16 cadence)
{
    cadenceSec = cadence/60;
    distance = (armLength*2*M_PI)*cadenceSec*timeBetween;
    voltageAccu = 0;
    
    switch (Pstate)
    {
        case START:
            for (size_t i = 0; i < sampleCount; i++)
            {
                voltageAccu += effectSampleValue[i];
            }
            averageVoltage = voltageAccu/sampleCount;
            effect = round(((averageVoltage/calibration) * pow(distance,2)) / pow(timeBetween,3));
                                 // force in kg           distance covered     time since broadcast
            if (sampleCount == arraySize-1)
            {
                Pstate = NORMAL;
            }
            break;
        
        case NORMAL:
            for (size_t i = 0; i < arraySize; i++)
            {
                voltageAccu += effectSampleValue[i];
            }
            averageVoltage = voltageAccu/sampleCount;
            effect = round(2 * (averageVoltage/calibration) * pow(distance,2) * pow(timeBetween,3));
            if (cadence < 10)
            {
                Pstate = START;
                sampleCount = 0;
            }
            break;
    }
    
    samplesSinceBroadcast = 0;
    
    return effect;
}

uint8 getSamplesSinceBroadcast(void)
{
    return samplesSinceBroadcast;
}

uint8 getSamplesPrBroadcast(void)
{
    return samplesPrBroadcast;
}

/* [] END OF FILE */
