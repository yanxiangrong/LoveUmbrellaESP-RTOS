//
// Created by Yan_X on 2022/3/28.
//

#ifndef ESP_RTOS_USER_CONFIG_H
#define ESP_RTOS_USER_CONFIG_H

#include "esp_common.h"

char DEVICE_ID[20] = {};
int32_t DEVICE_ID_INT = 0;

void init_id() {
    DEVICE_ID_INT = (int32_t) (system_get_chip_id() & ~(1 << 31));
    sprintf(DEVICE_ID, "%u", DEVICE_ID_INT);
}

#endif //ESP_RTOS_USER_CONFIG_H
