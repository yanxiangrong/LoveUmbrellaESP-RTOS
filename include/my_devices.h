//
// Created by Yan_X on 2022/4/1.
//

#ifndef ESP_RTOS_MY_DEVICES_H
#define ESP_RTOS_MY_DEVICES_H

#include "esp_common.h"
#include "pcf8575.h"


#define PIN_LOCK 0
#define PIN_LOCK_PWM 14
#define PIN_LCD 1
#define PIN_LED 2
#define PIN_LCD_POWER_BUTTON 3

struct {
    bool ad_lcd;
    bool led;
    bool lock;
}dev_status;

bool open_lock() {
    if (dev_status.lock) {
        return true;
    }

    bool res;

    setGPIO(PIN_LOCK, HIGH);
    res = updateGPIO();
    if (not res) {
        return false;
    }

    // todo
}

bool close_lock() {
    if (not dev_status.lock) {
        return true;
    }

    bool res;

    setGPIO(PIN_LOCK, HIGH);
    res = updateGPIO();
    if (not res) {
        return false;
    }

    // todo
}

bool open_ad_lcd() {
    if (dev_status.ad_lcd) {
        return true;
    }

    bool res;

    setGPIO(PIN_LCD, HIGH);
    setGPIO(PIN_LCD_POWER_BUTTON, LOW);
    res = updateGPIO();

    if (not res) return false;

    vTaskDelay(1000 / portTICK_RATE_MS);
    setGPIO(PIN_LCD_POWER_BUTTON, HIGH);

    return updateGPIO();
}

bool close_ad_lcd() {
    if (not dev_status.ad_lcd) {
        return true;
    }

    setGPIO(PIN_LCD, LOW);

    return updateGPIO();
}

bool open_led() {
    if (dev_status.led) {
        return true;
    }

    setGPIO(PIN_LED, HIGH);

    return updateGPIO();
}

bool close_led() {
    if (not dev_status.led) {
        return true;
    }

    setGPIO(PIN_LED, LOW);

    return updateGPIO();
}

#endif //ESP_RTOS_MY_DEVICES_H
