//
// Created by Yan_X on 2022/3/22.
//

#ifndef ESP_RTOS_RESTORE_H
#define ESP_RTOS_RESTORE_H


#include "esp_common.h"
#include "esp_wifi.h"
#include "crc32.h"

#define RTC_MEMORY_START 65

typedef struct {
    int resetCount;
} StoreData;

typedef struct {
    StoreData storeData;
    uint32_t crc;
} RtcStore;

void clear_reset_count();

void write_reset_count(int);

int read_reset_count() {
    RtcStore rtcMem;
    uint32_t crc;

    system_rtc_mem_read(RTC_MEMORY_START, &rtcMem, sizeof(rtcMem));

    crc = crc32((const char *) &rtcMem.storeData, sizeof(rtcMem.storeData));
    if (rtcMem.crc != crc) {
        printf("Rtc store crc check fail!\n");
        write_reset_count(1);
        return 0;
    }

    return rtcMem.storeData.resetCount;
}

void write_reset_count(int count) {
    RtcStore rtcMem = {};
    rtcMem.storeData.resetCount = count;

    rtcMem.crc = crc32((const char *) &rtcMem.storeData, sizeof(rtcMem.storeData));

    system_rtc_mem_write(RTC_MEMORY_START, &rtcMem, sizeof(rtcMem));
}

void clear_reset_count() {
    write_reset_count(0);
    printf("Clear reset count.\n");
}

void check_count() {
    int count;
    count = read_reset_count();
    printf("Reset count %d\n", count);
    write_reset_count(count + 1);
    if (count >= 5) {
        printf("Restore to factory settings.\n");
        clear_reset_count();
        system_restore();
        system_restart();
    }
}

void task_reset_count() {
    check_count();

    vTaskDelay(5000 / portTICK_RATE_MS);

    write_reset_count(1);

    vTaskDelete(NULL);
}

#endif //ESP_RTOS_RESTORE_H
