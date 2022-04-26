//
// Created by Yan_X on 2022/4/1.
//

#ifndef ESP_RTOS_MY_DEVICES_H
#define ESP_RTOS_MY_DEVICES_H

#include "esp_common.h"


#define PIN_LOCK_PWM 12
#define PIN_LOCK               0
#define PIN_LCD                1
#define PIN_LED                2
#define PIN_LCD_POWER_BUTTON   3
#define PIN_DETECT             4

void lock_init();

bool open_lock();

bool close_lock();

bool open_ad_lcd();

bool close_ad_lcd();

bool open_led();

bool close_led();

void auto_lock();

#endif //ESP_RTOS_MY_DEVICES_H
