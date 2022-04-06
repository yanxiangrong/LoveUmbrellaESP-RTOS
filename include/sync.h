//
// Created by Yan_X on 2022/3/27.
//

#ifndef ESP_RTOS_SYNC_H
#define ESP_RTOS_SYNC_H

#include "esp_common.h"

void sync_init();

void detect_ok();

void detect_ok_isr();

void wait_detect();

void try_detect();

void network_ok();

void wait_network();

void kcp_ok();

void wait_kcp();

void time_ok();

void wait_time();


#endif //ESP_RTOS_SYNC_H
