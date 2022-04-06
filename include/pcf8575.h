//
// Created by Yan_X on 2022/3/31.
//

#ifndef ESP_RTOS_PCF8575_H
#define ESP_RTOS_PCF8575_H

#include "esp_common.h"


#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

bool readGPIO(int pin);

bool updateGPIO();

bool write_gpio_data();

bool read_gpio_data();

void setGPIO(int pin, bool value);

void pcf8575_init(void);


#endif //ESP_RTOS_PCF8575_H
