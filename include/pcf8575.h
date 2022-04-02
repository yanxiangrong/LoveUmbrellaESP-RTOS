//
// Created by Yan_X on 2022/3/31.
//

#ifndef ESP_RTOS_PCF8575_H
#define ESP_RTOS_PCF8575_H

#include "esp_common.h"
#include "i2c.h"

#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

uint16_t gpio_value = 0xFFFF;
uint16_t gpio_status = 0xFFFF;


bool updateGPIO() {
    uint8_t readData[2] = {};

    i2c_master_start();
    i2c_master_writeByte(0x4E);
    if (!i2c_master_checkAck()) {
        printf("I2C_SET_DEV_FAILED\n");
        return false;
    }
    i2c_master_writeByte((uint8_t) gpio_value);
    if (!i2c_master_checkAck()) {
        printf("I2C_WRITE_BYTE_FAILED\n");
        return false;
    }
    i2c_master_writeByte((uint8_t) (gpio_value >> 8));
    if (!i2c_master_checkAck()) {
        printf("I2C_WRITE_BYTE_FAILED\n");
        return false;
    }
    i2c_master_stop();

    i2c_master_start();
    i2c_master_writeByte(0x4F);
    if (!i2c_master_checkAck()) {
        printf("I2C_SET_DEV_FAILED\n");
        return false;
    }
    readData[0] = i2c_master_readByte();
    i2c_master_setAck(0);
    readData[1] = i2c_master_readByte();
    i2c_master_setAck(1);
    i2c_master_stop();

    gpio_status |= readData[0];
    gpio_status |= ((uint16_t) readData[1]) << 8;
    gpio_status &= readData[0];
    gpio_status &= ((uint16_t) readData[1]) << 8;

    return true;
}

void setGPIO(int pin, bool value) {
    if (value) {
        gpio_value |= 1 << pin;
    } else {
        gpio_value &= ~(1 << pin);
    }
}

bool readGPIO(int pin) {
    return gpio_status & 1 << pin;
}

#endif //ESP_RTOS_PCF8575_H
