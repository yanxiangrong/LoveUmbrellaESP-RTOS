//
// Created by Yan_X on 2022/3/22.
//

#ifndef ESP_RTOS_RESTORE_H
#define ESP_RTOS_RESTORE_H


#include "esp_common.h"

#define RTC_MEMORY_START 65

typedef struct {
    int resetCount;
} StoreData;

typedef struct {
    StoreData storeData;
    uint32_t crc;
} RtcStore;

void clear_reset_count();

void task_reset_count();

#endif //ESP_RTOS_RESTORE_H
