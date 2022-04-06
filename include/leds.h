//
// Created by Yan_X on 2022/3/21.
//

#ifndef ESP_RTOS_LEDS_H
#define ESP_RTOS_LEDS_H

#include "esp_common.h"
#include "gpio.h"

#define HIGH 1
#define LOW 0
#define LED_BUILTIN GPIO_Pin_2

enum LedStatus {
    LED_OFF, LED_ALWAYS_ON, LED_BLINK_FAST, LED_BLINK_ONCE
};

void led_blink_fast();

void init_led();

void led_always_on();

void led_blink_once();

void led_off();

#endif //ESP_RTOS_LEDS_H
