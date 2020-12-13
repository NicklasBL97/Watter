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
#include "ADXL345Sensor.h"
#include "watUtility.h"
#include <math.h>
#include "my_I2C.h"
#include "stdio.h"

// movAvg variabler --
#define M 10
uint8 indexCounterMovAvg=0;
float filterArray[M];
//

ADXL345I2CData I2CData;
// DETTE ER TEST VARIABLER
uint16 grader = 0;
uint16 totalGrader = 0;
uint8_t antalomgange = 0;
float xAxis_p1p2[2]={0,0},yAxis_p1p2[2]={0,0},zAxis_p1p2[2]={1,1};
// SLUTTER HER

void ADXL345Init()
{
    cy_stc_scb_i2c_context_t i2cContext;
    Cy_SCB_I2C_Disable(I2C_HW, &i2cContext);
    const cy_stc_scb_i2c_config_t i2cConfig =
    {
        .i2cMode   = CY_SCB_I2C_MASTER,
        .useRxFifo = true,
        .useTxFifo = true,
        .slaveAddress     = 0U,
        .slaveAddressMask = 0U,
        .acceptAddrInFifo = false,
        .ackGeneralAddr   = false,
        .enableWakeFromSleep = false,
        .enableDigitalFilter = false,
        .lowPhaseDutyCycle = 8U,
        .highPhaseDutyCycle = 8U,
    };
    (void) Cy_SCB_I2C_Init(I2C_HW, &i2cConfig, &i2cContext);
    NVIC_EnableIRQ((IRQn_Type) I2C_SCB_IRQ_cfg.intrSrc);
    Cy_SCB_I2C_Enable(I2C_HW);
    
    // initialisering
    //Register 0x2C er BW-Rata som bestemmer båndbredden og data output rate. 
    writeRegister(0x2C,11);
    //Register 0x2D er Power control som bestemmer hvilket mode den er i f.eks (sleep mode eller measure mode). 
    writeRegister(0x2D, 8);
    //Register 0x31 er Data format som bestemmer hvilken resolution vi bruger. 
    writeRegister(0x31,9);
    //Regiseter 0x27 er Activate/inactivatete control. Her aktiveres de akser vi vil læse fra. 
    writeRegister(0x27,112);
    
    // Calibration
    // X-Axis
    writeRegister(0x1E,-2);
    // Y-Axis 
    writeRegister(0x1F,0);
    // Z-Axis
    writeRegister(0x20,13);
    
}
float deltaAxis2Deg(float xData1,float yData1, float zData1, 
                    float xData2, float yData2, float zData2)
{
    // Beregner vinklen i grader, mellem 2 accelerometer
    // målinger. Testet og virker i visual studio.
    
    // 3D pythagoras
    float ZXY1 = sqrt((pow(xData1,2)+pow(yData1,2)+pow(zData1,2)));
    float ZXY2 = sqrt((pow(xData2,2)+pow(yData2,2)+pow(zData2,2)));
   
    // "acos( ( X1*X2 + Y1*Y2 + Z1*Z2 ) /  ZXY1 * ZXY2)" ...
    // ... giver vinklen mellem de 2 punkter i radianer.
    
    // Det ganges med 180/pi for at omregne til deg.
	float vinkelRad = acos(((xData1 * xData2) + (yData1 * yData2) + (zData1 * zData2)) / (ZXY1 * ZXY2));
	float vinkelDeg = vinkelRad * (180.0 / M_PI);
	
    return vinkelDeg;
}

float RPM(float x, float y, float z)
{
    // test variable
    xAxis_p1p2[0] = x;
    yAxis_p1p2[0] = y;
    zAxis_p1p2[0] = z;
    
    // Grader drejet = antal grader allerede drejet + nyt antal grader.
    grader = deltaAxis2Deg(xAxis_p1p2[0],yAxis_p1p2[0],zAxis_p1p2[0],
                    xAxis_p1p2[1],yAxis_p1p2[1],zAxis_p1p2[1]);
    totalGrader += grader;
    
    // for at tælle antal omgange
    if(totalGrader>=360)
    {
        antalomgange++;
        totalGrader = 0;
    }
    
    // Rykker data en plads i arrayet
    // så der kan samles et nyt punkt. P1 = P0, P0 = ny værdi næste omgang i loop.
    xAxis_p1p2[1]=xAxis_p1p2[0];
    yAxis_p1p2[1]=yAxis_p1p2[0];
    zAxis_p1p2[1]=zAxis_p1p2[0];
    
    float omgang=(grader/360.0)/(1/20.0);
    float RPM = omgang*60.0; 
    
    float avgRpm = average(RPM);
    
    //printf(" \tGrader: %d ", grader);
    //printf("\tAntalRPM:\t%.1f \r\n", avgRpm);
    
    return avgRpm;
}
float average(float axisData)
{  
    float sum = 0;
    // circ buffer
    if(indexCounterMovAvg % M-1 == 1)
    {
        filterArray[indexCounterMovAvg] = axisData/M;
        indexCounterMovAvg = 0;
    }
    else
    {
        filterArray[indexCounterMovAvg] = axisData/M;
        indexCounterMovAvg++;
    }
    
    for(size_t i=0; i<M; i++)
    {
        sum += filterArray[i];   
    }
    return sum;
    
}

ADXL345Data ADXL345GetData(){
        
    I2CData.x[0] = readRegister(0x32);
    I2CData.x[1] = readRegister(0x33);
    
    I2CData.y[0] = readRegister(0x34);
    I2CData.y[1] = readRegister(0x35);
    
    I2CData.z[0] = readRegister(0x36);
    I2CData.z[1] = readRegister(0x37);
    
    ADXL345Data data;
    
    data.x = ((I2CData.x[0] | I2CData.x[1] << 8) / 256.0);
    data.y = ((I2CData.y[0] | I2CData.y[1] << 8) / 256.0);
    data.z = ((I2CData.z[0] | I2CData.z[1] << 8) / 256.0);
    
    return data;
}

/* [] END OF FILE */
