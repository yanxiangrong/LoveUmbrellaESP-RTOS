//
// Created by Yan_X on 2022/3/21.
//

#ifndef ESP_RTOS_UITLS_H
#define ESP_RTOS_UITLS_H

#include "esp_common.h"
#include "time.h"

#define not !
#define and &&
#define or ||

#define TRUE_STRING "True"
#define FALSE_STRING "False"

#define COMMON_YEAR_SECONDS 31536000
#define COMMON_YEAR_FEB_SECONDS 2419200
#define LEAP_YEAR_SECONDS 31622400
#define LEAP_YEAR_FEB_SECONDS 2505600
#define SMALL_MONTH_SECONDS 2592000
#define BIG_MONTH_SECONDS 2678400
#define DAY_SECONDS 86400
#define HOUR_SECONDS 3600


char *bool_to_str(bool val);

char *int64_to_str(int64_t val);

char *int64_to_str_comma(int64_t val);

int64_t time_now();

int64_t time_rtc_now();

void update_time(int64_t newTime);

int32_t tmToTimestamp(const struct tm *time);

void timestampToTm(int32_t timestamp, struct tm *time);

bool hostnameToIp(const char *hostname, ip_addr_t *ipAddr);

#endif //ESP_RTOS_UITLS_H
