/******************************************************************************
 * I²C driver for ESP8266_RTOS_SDK
 * 
 * smaple defines for BME280
 * -DI2C_MASTER_SDA_GPIO=5 -DI2C_MASTER_SCL_GPIO=4 -DI2C_MASTER_HALF_CYCLE=10
 * 
*******************************************************************************/
#pragma once

#include <esp_common.h>

/**
 * @brief return codes
 */
enum {
    I2C_OK = 0,
    I2C_SET_DEV_FAILED = -1,
    I2C_SET_REGISTER_FAILED = -2,
    I2C_START_AGAIN_FAILED = -3,
    I2C_READ_BYTE_FAILED = -4,
    I2C_WRITE_BYTE_FAILED = -5
} I2C_ERROR;

void i2c_master_gpio_init(void);
void i2c_master_init(void);

#define i2c_master_wait(x)    os_delay_us(x)

void i2c_master_stop(void);
void i2c_master_start(void);
void i2c_master_setAck(uint8_t level);
uint8_t i2c_master_getAck(void);
uint8_t i2c_master_readByte(void);
void i2c_master_writeByte(uint8_t wrdata);

bool i2c_master_checkAck(void);
void i2c_master_send_ack(void);
void i2c_master_send_nack(void);

/**
 * Reads a given register
 */
bool i2c_master_readRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t *regValue);

/**
 * Writes a given register
 */
bool i2c_master_writeRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t regValue);

/******************************************************************************
 * FunctionName : i2c_master_read_bytes
 * Description  : read bytes from dev_id at register reg_addr
 * Parameters   : dev_id the address of the device (unshifted [0..0x7f])
 * Parameters   : reg_addr the register to read
 * Parameters   : reg_data array to store read bytes
 * Parameters   : len count of bytes to read
 * Returns      : I2C_ERROR
 *******************************************************************************/
int8_t i2c_master_read_bytes(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

/******************************************************************************
 * FunctionName : i2c_master_write_bytes
 * Description  : writes bytes to dev_id at register reg_addr
 * Parameters   : dev_id the address of the device (unshifted [0..0x7f])
 * Parameters   : reg_addr the register to read
 * Parameters   : reg_data array of bytes
 * Parameters   : len count of bytes to write
 * Returns      : I2C_ERROR
 *******************************************************************************/
int8_t i2c_master_write_bytes(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
