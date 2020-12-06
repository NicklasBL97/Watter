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
#include "sensor.h"
#include "my_I2C.h"


void initialisering()
{
    cy_stc_scb_i2c_context_t i2cContext;
    Cy_SCB_I2C_Disable(I2C_HW, &i2cContext);
    const cy_stc_scb_i2c_config_t i2cConfig =
    {
        .i2cMode   = CY_SCB_I2C_MASTER,
        .useRxFifo = false,
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
    writeRegister(0x2C,6);
    //Register 0x2D er Power control som bestemmer hvilket mode den er i f.eks (sleep mode eller measure mode). 
    writeRegister(0x2D, 8);
    //Register 0x31 er Data format som bestemmer hvilken resolution vi bruger. 
    writeRegister(0x31,9);

    //Regiseter 0x27 er Activate/inactivatete control. Her aktiveres de akser vi vil læse fra. 
    writeRegister(0x27,112);

    // Calibration
    // X-Axis
    writeRegister(0x1E,0);
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

/* [] END OF FILE */
