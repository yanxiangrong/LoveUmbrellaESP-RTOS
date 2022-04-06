//
// Created by Yan_X on 2022/3/21.
//

#ifndef ESP_RTOS_WIFI_H
#define ESP_RTOS_WIFI_H


#include "esp_common.h"


#define DEVICE_TYPE         "gh_9e2cff3dfa51"  //wechat public number

#define DEFAULT_LAN_PORT    12476


void task_smartconfig(void *ignore);

#endif //ESP_RTOS_WIFI_H
