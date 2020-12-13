/**
 * @file ADXL345Sensor.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once
#include "project.h"

/**
 * @brief This struct is used when getting raw data from I2C communication with ADXL345
 * 
 */
typedef struct ADXL345I2CData{
    int8 x[2];
    int8 y[2];
    int8 z[2];
}ADXL345I2CData;

/**
 * @brief This struct is used when data is restructured from the raw I2C data struct
 * 
 */
typedef struct ADXL345Data{
    float x;
    float y;
    float z;
}ADXL345Data;

/**
 * @brief 
 * 
 */
void ADXL345Init();

/**
 * @brief 
 * 
 * @param xData1 
 * @param yData1 
 * @param zData1 
 * @param xData2 
 * @param yData2 
 * @param zData2 
 * @return float 
 */
float deltaAxis2Deg(float xData1,float yData1, float zData1, float xData2, float yData2, float zData2);

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @param z 
 * @return float 
 */
float RPM(float x, float y, float z);

/**
 * @brief 
 * 
 * @return float 
 */
float average(float);

/**
 * @brief 
 * 
 * @return ADXL345Data 
 */
ADXL345Data ADXL345GetData();



/* [] END OF FILE */
