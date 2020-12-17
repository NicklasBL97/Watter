/**
 * @file my_I2C.h
 * @author your name (you@domain.com)
 * @brief This Header includes helper functions for implementing I2C parts of the project. See ADXL345Sensor.h
 * @version 0.1
 * @date 2020-12-13
 * 
 * 
 * 
 */
#pragma once
#include "project.h"

/**
 * @brief This value is the address of the I2C slave unit which in this case is ADXL 345
 * 
 */
#define addrADXL (0x53)
/**
 * @brief This value is the timeout. Used in the I2C functions used ind both \p writeRegister & \p readRegister . 
 * 
 */
#define TIMEOUT 100

/**
 * @brief Status contains cy_en_scb_i2c_status_t as a simpler expression
 * 
 */
cy_en_scb_i2c_status_t  status;

/**
 * @brief Error handleling function 
 * 
 * This error handleling function prints out \p errnum 
 * along with a message that an error has occurred. 
 * This is done by using the LOG() function. 
 * 
 * @param errnum Error number. The function when called is handed a number, thus you know where the function is called from. 
 */
void waitForOperation(uint8_t errnum);

/**
 * @brief I2C write function 
 * 
 * This function starts the I2C communication, 
 * defines that it is at write transfer 
 * and stops the communication after it is done.  
 * 
 * @param reg_addr Register address of the register the user wants to write to.  
 * @param data Data is the argument you want to write to the given register in \p reg_addr. 
 */
void writeRegister(uint8_t reg_addr, int8_t data);

/**
 * @brief I2C read function.
 *
 * This function starts I2C communication,
 * sets the address to read from, and read from the specified address
 * and stops communication when a byte has been read.
 *
 * @param reg_addr The register to read data from
 * @return uint8 Data read from the specified register address
 */
uint8 readRegister(uint8 reg_addr);

/* [] END OF FILE */
