//
// Created by Yan_X on 2022/4/1.
//

#ifndef ESP_RTOS_MY_DEVICES_H
#define ESP_RTOS_MY_DEVICES_H

#include "esp_common.h"
#include "pcf8575.h"


#define PIN_LOCK 0
#define PIN_LOCK_PWM 12
#define PIN_DETECT 14
#define PIN_LCD 1
#define PIN_LED 2
#define PIN_LCD_POWER_BUTTON 3

struct {
    bool ad_lcd;
    bool led;
    bool lock;
} dev_status = {};


void lock_init() {
    uint32 duty[4] = {0, 0};
    uint32 io_info[4][3] = {
            // MUX, FUNC, PIN
            {PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12, 12},
            {PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13, 13},
    };

    pwm_init(20000, duty, 2, io_info);
    pwm_start();
}

bool open_lock() {
    if (dev_status.lock) {
        return true;
    }
    xSemaphoreTake(networkMutex, 0);

    bool res;

    setGPIO(PIN_LOCK, HIGH);
    res = updateGPIO();
    if (not res) {
        return false;
    }

    pwm_set_duty(1024 / 40, 0);
    pwm_start();

    vTaskDelay(1000 / portTICK_RATE_MS);

    dev_status.lock = true;
    return true;
}

bool close_lock() {
    if (not dev_status.lock) {
        return true;
    }

    bool res;

    pwm_set_duty(1024 / 40 * 4, 0);
    pwm_start();

    vTaskDelay(1500 / portTICK_RATE_MS);

    setGPIO(PIN_LOCK, LOW);
    res = updateGPIO();
    if (not res) {
        return false;
    }

    dev_status.lock = false;
    return true;
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

    res = updateGPIO();
    if (not res) return false;

    dev_status.ad_lcd = true;
    return true;
}

bool close_ad_lcd() {
    if (not dev_status.ad_lcd) {
        return true;
    }

    bool res;

    setGPIO(PIN_LCD, LOW);

    res = updateGPIO();

    if (not res) return false;

    dev_status.ad_lcd = false;
    return true;
}

bool open_led() {
    if (dev_status.led) {
        return true;
    }

    bool res;

    setGPIO(PIN_LED, HIGH);

    res = updateGPIO();

    if (not res) return false;

    dev_status.led = true;
    return true;
}

bool close_led() {
    if (not dev_status.led) {
        return true;
    }

    bool res;

    setGPIO(PIN_LED, LOW);

    res = updateGPIO();

    if (not res) return false;

    dev_status.led = false;
    return true;
}

void auto_lock_handle() {
    wait_detect();
    vTaskDelay(500 / portTICK_RATE_MS);
    printf("detect, lock!\n");
    close_lock();
    vTaskDelete(NULL);
}

void auto_lock() {
    xTaskCreate(&auto_lock_handle, (const signed char *) "auto_lock", 512, NULL, 3, NULL);
}

#endif //ESP_RTOS_MY_DEVICES_H
