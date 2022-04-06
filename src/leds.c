//
// Created by Yan_X on 2022/4/5.
//

#include "leds.h"
#include "freertos/semphr.h"



xTaskHandle blinkFastTaskHandle = 0;
xTaskHandle blinkOnceTaskHandle = 0;
xQueueHandle blinkOnceHandle = 0;
enum LedStatus ledStatus = LED_OFF;


void task_blink_once(void *);


void init_led() {
    GPIO_AS_OUTPUT(LED_BUILTIN);
    ledStatus = LED_ALWAYS_ON;

    blinkOnceHandle = xSemaphoreCreateCounting(5, 0);
    xTaskCreate(&task_blink_once, (const signed char *) "blink_once", 512, NULL, 3, blinkOnceTaskHandle);
}


void set_builtin_led_on() {
    GPIO_OUTPUT(GPIO_Pin_2, LOW);
}


void set_builtin_led_off() {
    GPIO_OUTPUT(GPIO_Pin_2, HIGH);
}


void led_prepare() {
    if (ledStatus == LED_OFF)
        return;

    switch (ledStatus) {
        case LED_BLINK_FAST:
            vTaskDelete(blinkFastTaskHandle);
            break;
        default:
            break;
    }
    set_builtin_led_off();
}


_Noreturn void task_blink_fast(void *ignore) {
    portTickType xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    while (true) {
        set_builtin_led_on();
        vTaskDelayUntil(&xLastWakeTime, 250 / portTICK_RATE_MS);

        set_builtin_led_off();
        vTaskDelayUntil(&xLastWakeTime, 250 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}


_Noreturn void task_blink_once(void *ignore) {
    while (true) {
        xSemaphoreTake(blinkOnceHandle, portMAX_DELAY);
        if (ledStatus != LED_BLINK_ONCE) {
            continue;
        }

        set_builtin_led_on();
        vTaskDelay(25 / portTICK_RATE_MS);
        set_builtin_led_off();
        vTaskDelay(25 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}


void led_blink_fast() {
    int result;

    led_prepare();

    result = xTaskCreate(&task_blink_fast, (const signed char *) "blink_fast", 512, NULL, 3, &blinkFastTaskHandle);
    if (result == pdTRUE) {
        ledStatus = LED_BLINK_FAST;
    }
}


void led_always_on() {
    led_prepare();
    set_builtin_led_on();
    ledStatus = LED_ALWAYS_ON;
}



void led_blink_once() {
    if (ledStatus != LED_OFF) {
        led_prepare();
    }
    ledStatus = LED_BLINK_ONCE;
    xSemaphoreGive(blinkOnceHandle);
}


void led_off() {
    led_prepare();
    ledStatus = LED_OFF;
}
