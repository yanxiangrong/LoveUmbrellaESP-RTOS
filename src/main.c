#include "esp_common.h"

#include "ikcp.h"
#include "leds.h"
#include "wifi.h"
#include "restore.h"
#include "sync.h"
#include "network.h"
#include "i2c.h"
#include "ds1307.h"
#include "ntp.h"
#include "m_time.h"

_Noreturn void task_print_meminfo(void *ignore) {
    portTickType xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    while (true) {
        system_print_meminfo();
        printf("Free heap size: %d\n\n", system_get_free_heap_size());
        vTaskDelayUntil(&xLastWakeTime, 5000 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void) {
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;
        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void) {
    printf("\n\n");
    printf("SDK version:%s\n", system_get_sdk_version());

    espconn_init();
    init_led();
    sync_init();
    i2c_master_gpio_init();
    dns_init();
    ntp_init();


    wifi_set_sleep_type(LIGHT_SLEEP_T);
    xTaskCreate(&task_print_meminfo, (const signed char *) "meminfo", 512, NULL, 1, NULL);
    xTaskCreate(&task_smartconfig, (const signed char *) "smartconfig", 1024, NULL, 2, NULL);
    xTaskCreate(&task_kcp, (const signed char *) "kcp", 512, NULL, 1, NULL);
    xTaskCreate(&task_report, (const signed char *) "ping", 512, NULL, 1, NULL);
    xTaskCreate(&task_kcp_recv, (const signed char *) "kcp_recv", 512, NULL, 1, NULL);
    xTaskCreate(&task_reset_count, (const signed char *) "reset_count", 512, NULL, 3, NULL);
    xTaskCreate(&task_update_time, (const signed char *) "ds1307", 512, NULL, 3, NULL);

}
