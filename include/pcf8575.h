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

bool readGPIO(int pin);

bool updateGPIO();

bool write_gpio_data() {
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
}

bool read_gpio_data() {
    uint8_t readData[2] = {};


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
    gpio_status &= 0xFF00 | readData[0];
    gpio_status &= ((uint16_t) readData[1]) << 8 | 0x00FF;

    return true;
}

void gpio_intr_handler(void) {
    _xt_isr_mask(1 << ETS_GPIO_INUM);    //disable interrupt

    read_gpio_data();

    if (readGPIO(4) == true) {
        detect_ok_isr();
    }

    printf("GPIO : 00 01 02 03 04 05 06 07 10 11 12 13 14 15 16 17\n");
    printf("LEVEL: ");
    for (int i = 0; i < 16; ++i) {
        printf("%c  ", readGPIO(i) ? 'H' : 'L');
    }
    printf("\n");

    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, GPIO_Pin_14); //clear interrupt mask
    _xt_isr_unmask(1 << ETS_GPIO_INUM); //Enable the GPIO interrupt
}


void set_intr(void) {
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(14));
    PIN_PULLUP_EN(GPIO_PIN_REG(14));

    gpio_pin_intr_state_set(GPIO_ID_PIN(14), GPIO_PIN_INTR_NEGEDGE);

    gpio_intr_handler_register(gpio_intr_handler, NULL);
    _xt_isr_unmask(1 << ETS_GPIO_INUM);    //Enable the GPIO interrupt
}

bool updateGPIO() {
    bool res;
    res = write_gpio_data();
    if (not res) {
        return false;
    }
    res = read_gpio_data();
    if (not res) {
        return false;
    }

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
    return gpio_status & (1 << pin);
}

void pcf8575_init(void) {
    for (int i = 5; i < 16; ++i) {
        setGPIO(i, LOW);
    }
    set_intr();
}

#endif //ESP_RTOS_PCF8575_H
