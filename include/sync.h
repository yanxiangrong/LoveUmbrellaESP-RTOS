//
// Created by Yan_X on 2022/3/27.
//

#ifndef ESP_RTOS_SYNC_H
#define ESP_RTOS_SYNC_H

#include "esp_common.h"
#include "freertos/semphr.h"

LOCAL xSemaphoreHandle networkMutex;
LOCAL xSemaphoreHandle kcpMutex;
LOCAL xSemaphoreHandle timeMutex;

void sync_init() {
    vSemaphoreCreateBinary(networkMutex)
    xSemaphoreTake(networkMutex, portMAX_DELAY);

    vSemaphoreCreateBinary(kcpMutex)
    xSemaphoreTake(kcpMutex, portMAX_DELAY);

    vSemaphoreCreateBinary(timeMutex)
    xSemaphoreTake(timeMutex, portMAX_DELAY);
}

void network_ok() {
    xSemaphoreGive(networkMutex);
}

void wait_network() {
    xSemaphoreTake(networkMutex, portMAX_DELAY);
    xSemaphoreGive(networkMutex);
}

void kcp_ok() {
    xSemaphoreGive(kcpMutex);
}

void wait_kcp() {
    xSemaphoreTake(kcpMutex, portMAX_DELAY);
    xSemaphoreGive(kcpMutex);
}

void time_ok() {
    xSemaphoreGive(timeMutex);
}

void wait_time() {
    xSemaphoreTake(timeMutex, portMAX_DELAY);
    xSemaphoreGive(timeMutex);
}

#endif //ESP_RTOS_SYNC_H
