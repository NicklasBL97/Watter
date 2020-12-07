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
uint8_t readByte;

void waitForOperation(uint8_t errnum)
{
    // Error handle for conditions were master is not ready
    //CyDelayUs(2);
    LOG("Fejl %d", errnum);
}

//for at skrive til et register vil vi gerne skrive to værdier. 
// En addresse og en indstilling i form af data.
void writeRegister(uint8_t reg_addr, int8_t data)
{
    //start kommunikation 
    status = Cy_SCB_I2C_MasterSendStart(I2C_HW,addrADXL,CY_SCB_I2C_WRITE_XFER,TIMEOUT,&I2C_context);
    if(status != CY_SCB_I2C_SUCCESS)
    {
        waitForOperation(1);
    }
    
    Cy_SCB_I2C_MasterReadByte(I2C_HW,CY_SCB_I2C_NAK,&readByte,TIMEOUT,&I2C_context);
    if(status != CY_SCB_I2C_SUCCESS)
    {
        waitForOperation(8);
    }
    // send byte til register
    Cy_SCB_I2C_MasterWriteByte(I2C_HW,reg_addr,TIMEOUT,&I2C_context);
    if(status != CY_SCB_I2C_SUCCESS)
    {
        waitForOperation(2);
    }
    Cy_SCB_I2C_MasterWriteByte(I2C_HW,data,TIMEOUT,&I2C_context);
    if(status != CY_SCB_I2C_SUCCESS)
    {
        waitForOperation(3);
    }
    
    //stop kommunikation
    I2C_MasterSendStop(TIMEOUT);
    if(status != CY_SCB_I2C_SUCCESS)
    {
        waitForOperation(4);
    }
}

// for at læse fra et register skal vi oplyse addressen på det register som vi gerne vil læse på. 

uint8 readRegister(uint8 reg_addr)
{
    
    //start kommunikation
    status = Cy_SCB_I2C_MasterSendStart(I2C_HW,addrADXL,CY_SCB_I2C_READ_XFER,TIMEOUT,&I2C_context);
    if(status != CY_SCB_I2C_SUCCESS)
    {
        waitForOperation(5);
    }
    // send byte til register for at starte kommunikation med bestemt register
    Cy_SCB_I2C_MasterWriteByte(I2C_HW,reg_addr,TIMEOUT,&I2C_context);
    if(status != CY_SCB_I2C_SUCCESS)
    {
        waitForOperation(6);
    }
    Cy_SCB_I2C_MasterReadByte(I2C_HW,CY_SCB_I2C_ACK,&readByte,TIMEOUT,&I2C_context);
    if(status != CY_SCB_I2C_SUCCESS)
    {
        waitForOperation(7);
    }
    Cy_SCB_I2C_MasterReadByte(I2C_HW,CY_SCB_I2C_NAK,&readByte,TIMEOUT,&I2C_context);
    if(status != CY_SCB_I2C_SUCCESS)
    {
        waitForOperation(8);
    }
    
    //stop kommunikation
    I2C_MasterSendStop(TIMEOUT);
    if(status != CY_SCB_I2C_SUCCESS)
    {
        waitForOperation(9);
    }
    return readByte;
}
/* [] END OF FILE */
