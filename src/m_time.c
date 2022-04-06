//
// Created by Yan_X on 2022/4/5.
//

#include "m_time.h"
#include "ds1307.h"
#include "uitls.h"
#include "sync.h"
#include "ntp.h"

void task_update_time(void *ignore) {
    bool res;
    struct tm time;

    time_t time1;

    res = ds1307_is_running();
    printf("Ds1307 is running: %s.\n", bool_to_str(res));

    if (not res) {
        if (not ds1307_start(true)) {
            printf("Ds1307 start fail!\n");
        } else {
            printf("Ds1307 reset!\n");
        }
    }
    ds1307_get_time(&time);

    time1 = tmToTimestamp(&time);
    update_time(((int64_t) time1) * 1000000);

    time_ok();

//    printf("%04d-%02d-%02d %02d:%02d:%02d \n",
//           time.tm_year, time.tm_mon + 1,
//           time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
    timestampToTm(time1, &time);
    printf("Ds1037 time: UTC %s", ctime(&time1));
    printf("Timestamp: %s\n", int64_to_str_comma(time1));

    wait_network();
    ntp_run();

    vTaskDelete(NULL);
}
