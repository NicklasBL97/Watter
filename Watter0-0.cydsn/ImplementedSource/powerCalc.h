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
#include "project.h"
#include "math.h"

#define FS 20               // Sample frequency
#define BROADCAST_FS 4      // Broadcast frequency
#define arraySize 2*FS

typedef enum { START, NORMAL } powerState;

void handleSample(uint16 powerVoltage);
uint16 getPower(int16 cadence);
uint8 getSamplesSinceBroadcast(void);
uint8 getSamplesPrBroadcast(void);

/* [] END OF FILE */
