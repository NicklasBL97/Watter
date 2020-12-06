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
#include "my_I2C.h"
#include "stdio.h"
#define addrADXL (0x53)

bool status;
static uint8 rbuff[2]; // Read buffer
static uint8 wbuff[2]; // write buffer
cy_en_scb_i2c_status_t  errorStatus;
cy_stc_scb_i2c_master_xfer_config_t register_setting;

void waitForOperation(uint8_t errnum)
{
    status = Cy_SCB_I2C_IsBusBusy(I2C_HW);
    printf("Fejl nummer: %d", errnum);
    while(status == true)
    {
        Cy_GPIO_Write(RED_PORT,RED_NUM,0);
        Cy_GPIO_Write(GREEN_PORT,GREEN_NUM,1);
        
        CyDelayUs(1); // venter på at status ikk er busy mere.
    }
    return;
}

//for at skrive til et register vil vi gerne skrive to værdier. 
// En addresse og en indstilling i form af data.

void writeRegister(uint8 reg_addr, int8 data)
{
    wbuff[0] = reg_addr;    //tildel det første element til register addressen.
    wbuff[1] = data;        //tildel det andet element til den data værdi som vi gerne vil bruge til at indstille registeret.
    
    register_setting.slaveAddress = addrADXL;
    register_setting.buffer = wbuff;
    register_setting.bufferSize = 2;
    register_setting.xferPending = false;
    
    errorStatus = Cy_SCB_I2C_MasterWrite(I2C_HW,&register_setting,&I2C_context);
    if(errorStatus == CY_SCB_I2C_SUCCESS)
    {
        CyDelayUs(5);        
    }
    waitForOperation(1);
}

// for at læse fra et register skal vi oplyse addressen på det register som vi gerne vil læse på. 

uint8 readRegister(uint8 reg_addr)
{
    wbuff[0] = reg_addr;
    
    register_setting.slaveAddress = addrADXL;
    register_setting.buffer = wbuff;
    register_setting.bufferSize = 1;
    register_setting.xferPending = true;
    
    Cy_SCB_I2C_MasterWrite(I2C_HW,&register_setting,&I2C_context);
    waitForOperation(2);

    register_setting.buffer = rbuff;
    register_setting.xferPending = false;
    
    Cy_SCB_I2C_MasterRead(I2C_HW,&register_setting,&I2C_context);
    waitForOperation(3);

    return rbuff[0];
}
/* [] END OF FILE */
