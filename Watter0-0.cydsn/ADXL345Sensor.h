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
#pragma once
#include "project.h"

typedef struct ADXL345I2CData{
    int8 x[2];
    int8 y[2];
    int8 z[2];
}ADXL345I2CData;

typedef struct ADXL345Data{
    int16 x;
    int16 y;
    int16 z;
}ADXL345Data;

void ADXL345Init();
float deltaAxis2Deg(float xData1,float yData1, float zData1, float xData2, float yData2, float zData2);
ADXL345Data ADXL345GetData();



/* [] END OF FILE */
