//
// Created by Yan_X on 2022/4/5.
//

#include "uitls.h"
#include "esp_common.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"



#define swap(x, y)   do{x=x+y;y=x-y;x=x-y;}while(0)
#define abs(x) ((x) > 0 ? (x) : (-x))
#define diff_abs(x, y) abs(a - y)


static char result_str[32] = {};
int64_t system_time_offset = 0;
int64_t system_rtc_time_offset = 0;


char *bool_to_str(bool val) {
    if (val) {
        strcpy(result_str, TRUE_STRING);
    } else {
        strcpy(result_str, FALSE_STRING);
    }
    return result_str;
}


char *int64_to_str(int64_t val) {
    int start = 0, end = -1;
    if (val == 0) {
        result_str[0] = '0';
        end++;
    }

    if (val < 0) {
        result_str[0] = '-';
        start++;
        end++;
        val = -val;
    }

    while (val) {
        end++;
        result_str[end] = (char) ((int) (val % 10) + '0');
        val /= 10;
    }
    result_str[end + 1] = 0;

    while (start < end) {
        swap(result_str[start], result_str[end]);
        start++;
        end--;
    }

    return result_str;
}


char *int64_to_str_comma(int64_t val) {
    int start = 0, end = -1, offset = 0;
    if (val == 0) {
        result_str[0] = '0';
        end++;
    }

    if (val < 0) {
        result_str[0] = '-';
        start++;
        end++;
        val = -val;
        offset = 1;
    }

    while (val) {
        end++;
        result_str[end] = (char) ((int) (val % 10) + '0');
        val /= 10;

        if (((end - offset + 2) % 4) == 0 and val != 0) {
            end++;
            result_str[end] = ',';
        }
    }
    result_str[end + 1] = 0;

    while (start < end) {
        swap(result_str[start], result_str[end]);
        start++;
        end--;
    }

    return result_str;
}


int64_t time_now() {
    return system_time_offset + system_get_time();
}


int64_t time_rtc_now() {
    return system_rtc_time_offset + system_get_rtc_time();
}


void update_time(int64_t newTime) {
    system_time_offset = newTime - system_get_time();
    system_rtc_time_offset = newTime - system_get_rtc_time();
}


bool isLeapYear(int year) {
    if (year % 4 == 0) {
        if (year % 100 == 0) {
            if (year % 400 == 0) {
                return true;
            }
            return false;
        }
        return true;
    } else {
        return false;
    }
}


int32_t tmToTimestamp(const struct tm *time) {
    int32_t timestamp = 0;

    for (int year = 1970; year < time->tm_year; ++year) {
        if (isLeapYear(year)) {
            timestamp += LEAP_YEAR_SECONDS;
        } else {
            timestamp += COMMON_YEAR_SECONDS;
        }
    }

    for (int month = 1; month < time->tm_mon + 1; ++month) {
        if (month == 2) {
            if (isLeapYear(time->tm_year)) {
                timestamp += LEAP_YEAR_FEB_SECONDS;
            } else {
                timestamp += COMMON_YEAR_FEB_SECONDS;
            }

            continue;
        }

        if (month % 2 == 0) {
            timestamp += SMALL_MONTH_SECONDS;
        } else {
            timestamp += BIG_MONTH_SECONDS;
        }
    }

    timestamp += DAY_SECONDS * (time->tm_mday - 1);
    timestamp += HOUR_SECONDS * (time->tm_hour);
    timestamp += 60 * time->tm_min;
    timestamp += time->tm_sec;
    timestamp -= 8 * HOUR_SECONDS;

    return timestamp;
}


void timestampToTm(int32_t timestamp, struct tm *time) {
    time->tm_year = 1970;
    time->tm_mon = 1;
    time->tm_mday = 1;
    time->tm_hour = 0;
    time->tm_min = 0;
    time->tm_sec = 0;
    time->tm_isdst = 0;

    timestamp += 8 * HOUR_SECONDS;

    for (int year = 1970; year <= 2050; ++year) {
        if (isLeapYear(year)) {
            if (timestamp >= LEAP_YEAR_SECONDS) {
                timestamp -= LEAP_YEAR_SECONDS;
            } else {
                time->tm_year = year;
                break;
            }
        } else {
            if (timestamp >= COMMON_YEAR_SECONDS) {
                timestamp -= COMMON_YEAR_SECONDS;
            } else {
                time->tm_year = year;
                break;
            }
        }
    }

    for (int month = 1; month <= 12; ++month) {
        if (month == 2) {
            if (isLeapYear(time->tm_year)) {
                if (timestamp >= LEAP_YEAR_FEB_SECONDS) {
                    timestamp -= LEAP_YEAR_FEB_SECONDS;
                } else {
                    time->tm_mon = month - 1;
                    break;
                }
            } else {
                if (timestamp >= COMMON_YEAR_SECONDS) {
                    timestamp -= COMMON_YEAR_FEB_SECONDS;
                } else {
                    time->tm_mon = month - 1;
                    break;
                }
            }
        }

        if (month % 2 == 0) {
            if (timestamp >= SMALL_MONTH_SECONDS) {
                timestamp -= SMALL_MONTH_SECONDS;
            } else {
                time->tm_mon = month - 1;
                break;
            }
        } else {
            if (timestamp >= BIG_MONTH_SECONDS) {
                timestamp -= BIG_MONTH_SECONDS;
            } else {
                time->tm_mon = month - 1;
                break;
            }
        }
    }

    time->tm_mday = timestamp / DAY_SECONDS + 1;
    timestamp %= DAY_SECONDS;
    time->tm_hour = timestamp / HOUR_SECONDS;
    timestamp %= HOUR_SECONDS;
    time->tm_min = timestamp / 60;
    timestamp %= 60;
    time->tm_sec = timestamp;
}


bool hostnameToIp(const char *hostname, ip_addr_t *ipAddr) {
    err_t err;
    int delay = 500;
    int failCount = 0;

    printf("DNS query for %s\n", hostname);
    while (failCount < 10) {
        err = dns_gethostbyname(hostname, ipAddr, NULL, NULL);
        if (err == ERR_OK) break;

        if (failCount > 5) {
            printf("DNS query fail.\n");
        }
        failCount++;
        delay += delay / 2;
        vTaskDelay(delay / portTICK_RATE_MS);
    }

    if (failCount == 10) {
        return false;
    } else {
        return true;
    }
}
