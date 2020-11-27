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
#include "printer.h"
#include "watUtility.h"
static uint8 rbuff[2]; // Read buffer
static uint8 wbuff[2]; // write buffer

static cy_stc_scb_i2c_master_xfer_config_t register_setting;

void waitForOperation()
{
    while(0 != (I2C_MasterGetStatus() &CY_SCB_I2C_MASTER_BUSY))
    {
        CyDelayUs(3); // venter på at status ikk er busy mere.
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
    I2C_MasterWrite(&register_setting);
    waitForOperation();
}

// for at læse fra et register skal vi oplyse addressen på det register som vi gerne vil læse på. 

uint8 readRegister(uint8 reg_addr)
{
    wbuff[0] = reg_addr;
    
    register_setting.slaveAddress = addrADXL;
    register_setting.buffer = wbuff;
    register_setting.bufferSize = 1;
    register_setting.xferPending = true;
    
    I2C_MasterWrite(&register_setting);
    waitForOperation();
    
    register_setting.buffer = rbuff;
    register_setting.xferPending = false;
    
    I2C_MasterRead(&register_setting);
    waitForOperation();
    
    return rbuff[0];
}
/* [] END OF FILE */
