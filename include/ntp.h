//
// Created by Yan_X on 2022/3/29.
//

#ifndef ESP_RTOS_NTP_H
#define ESP_RTOS_NTP_H

#define NTP_SERVER_PORT 123
#define NTP_TIMESTAMP_DELTA 2208988800ll
#define NTP_MICROS_DELTA 2208988800000000ll


void ntp_init();

void ntp_run();

_Noreturn void task_kcp_recv();

#endif //ESP_RTOS_NTP_H
