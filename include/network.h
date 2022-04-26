//
// Created by Yan_X on 2022/3/27.
//

#ifndef ESP_RTOS_NETWORK_H
#define ESP_RTOS_NETWORK_H

#include "cJSON.h"

_Noreturn void task_kcp();

void send_json(cJSON *json);

_Noreturn void task_report();

cJSON *comm_json();

_Noreturn void task_kcp_recv();

#endif //ESP_RTOS_NETWORK_H
