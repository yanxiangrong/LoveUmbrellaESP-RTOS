//
// Created by Yan_X on 2022/4/4.
//

#ifndef ESP_RTOS_CONFIG_H
#define ESP_RTOS_CONFIG_H


#include "esp_common.h"


struct {
    bool auto_update_time;
} device_config = {
        .auto_update_time = true
};

void config_init() {

}

void save_config() {
    system_param_save_with_protect()
}

#endif //ESP_RTOS_CONFIG_H
