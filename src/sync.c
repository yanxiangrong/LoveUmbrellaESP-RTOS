//
// Created by Yan_X on 2022/4/5.
//

#include "sync.h"
#include "freertos/semphr.h"


LOCAL xSemaphoreHandle networkMutex;
LOCAL xSemaphoreHandle kcpMutex;
LOCAL xSemaphoreHandle timeMutex;
LOCAL xSemaphoreHandle detectMutex;

void sync_init() {
    vSemaphoreCreateBinary(networkMutex)
    xSemaphoreTake(networkMutex, portMAX_DELAY);

    vSemaphoreCreateBinary(kcpMutex)
    xSemaphoreTake(kcpMutex, portMAX_DELAY);

    vSemaphoreCreateBinary(timeMutex)
    xSemaphoreTake(timeMutex, portMAX_DELAY);

    vSemaphoreCreateBinary(detectMutex)
    xSemaphoreTake(detectMutex, portMAX_DELAY);
}

void detect_ok() {
    xSemaphoreGive(networkMutex);
}

void detect_ok_isr() {
    static portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(networkMutex, &xHigherPriorityTaskWoken);
}

void wait_detect() {
    xSemaphoreTake(networkMutex, portMAX_DELAY);
}

void try_detect() {
    xSemaphoreTake(networkMutex, 0);
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
