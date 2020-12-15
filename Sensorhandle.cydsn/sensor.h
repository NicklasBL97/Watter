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
 * @brief Initialization function 
 * This function writes to a bunch of registers on the sensor, 
 * all of which is required for reading from the sensors three axises. 
 * It is also within this funciton the user is able to calibrate all of the sensors axises. 
 * 
 */
void ADXL345Init();

/**
 * @brief Calculates the number of degrees between two accelerometer datasets
 *
 * This function uses 3D vector math to calculate the of degrees between two accelerometer measurements.
 * Assumes that both dataset is under the same amout of G-force, 1G from gravity, 
 * and can only calculate positive degree values.
 *
 * @param xData1 The X value of dataset #1
 * @param yData1 The Y value of dataset #1
 * @param zData1 The Z value of dataset #1
 *
 * @param xData2 The X value of dataset #2
 * @param yData2 The Y value of dataset #2
 * @param zData2 The Z value of dataset #2
 * @return float The calculated number of degrees between dataset #1 and dataset #2
 */
float deltaAxis2Deg(float xData1,float yData1, float zData1, float xData2, float yData2, float zData2);

/**
 * @brief A moving average filter, used to smooth out the RPM calculation
 * 
 * Used inside RPM function, and uses a circular buffer to store RPM values and calculate the moving average of "M" values,
 * where "M" is the array size.
 *
 * @param rpmData Raw RPM value to be averaged.
 * @return float The calculated average of "M" number of raw RPM values.
 */
float average(float rpmData);

/**
 * @brief Calculates the revolutions pr. minute
 * 
 * This function uses both average() and deltaAxis2Deg() to return an average revolutions pr. minute.
 * by using timed interrupt and a single measurement dataset.
 * 
 * @param x The momentary value of the X Axis. 
 * @param y The momentary value of the y Axis.
 * @param z The momentary value of the Z Axis.
 * @return float a average revolutions pr. minute. 
 */
float RPM(float x, float y, float z);


/**
 * @brief When called, reads from the related I2C registers and stores the meassurement in "data" variable and processes them.
 *
 * Is responsible for getting X, Y and Z data from the accelerometer via the I2C read functions. 
 * The function divides the read values by the sensitivity of the accelerometer (typ. 256) 
 * and also stores the processed values in data.x, data.y and data.z.
 *
 * @return ADXL345Data Returns the ADXL data struct, containing the processed data.x, data.y and data.z
 */
ADXL345Data ADXL345GetData();


/* [] END OF FILE */
