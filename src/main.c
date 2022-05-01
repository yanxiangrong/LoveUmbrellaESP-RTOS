#include "espressif/c_types.h"
#include "lwip/ip4_addr.h"  //虽然没有直接使用这个头文件，但是删掉会编译失败
#include "espconn.h"
#include "esp_common.h"
#include "frequencies.h"
#include "my_devices.h"
#include "sync.h"
#include "user_config.h"
#include "i2c.h"
#include "ntp.h"
#include "leds.h"
#include "pcf8575.h"
#include "wifi.h"
#include "network.h"
#include "restore.h"
#include "m_time.h"
#include "lwip/dns.h"



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


_Noreturn void beep_test() {
    uint32 duty[4] = {512, 512};
    uint32 io_info[4][3] = {
            // MUX, FUNC, PIN
            {PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12, 12},
            {PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13, 13},
    };
    pwm_init(1000, duty, 2, io_info);
    pwm_start();
    vTaskDelay(2000 / portTICK_RATE_MS);

    while (true) {
        pwm_set_period(1000000 / C6);
        pwm_start();
        vTaskDelay(1000 / portTICK_RATE_MS);

        pwm_set_period(1000000 / D6);
        pwm_start();
        vTaskDelay(1000 / portTICK_RATE_MS);

        pwm_set_period(1000000 / E6);
        pwm_start();
        vTaskDelay(1000 / portTICK_RATE_MS);

        pwm_set_period(1000000 / F6);
        pwm_start();
        vTaskDelay(1000 / portTICK_RATE_MS);

        pwm_set_period(1000000 / G6);
        pwm_start();
        vTaskDelay(1000 / portTICK_RATE_MS);

        pwm_set_period(1000000 / A6);
        pwm_start();
        vTaskDelay(1000 / portTICK_RATE_MS);

        pwm_set_period(1000000 / B6);
        pwm_start();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}


_Noreturn void lock_test() {
    next:
    vTaskDelay(2000 / portTICK_RATE_MS);
    open_lock();
    vTaskDelay(2000 / portTICK_RATE_MS);
    close_lock();
    goto next;
    vTaskDelay(2000 / portTICK_RATE_MS);
    open_lock();

    while (true) {
        wait_detect();
        vTaskDelay(500 / portTICK_RATE_MS);
        printf("detect, lock!\n");
        close_lock();

        vTaskDelay(2000 / portTICK_RATE_MS);
        open_lock();
        printf("unlock, wait.\n");
    }

    vTaskDelete(NULL);
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

    xTaskCreate(&task_reset_count, (const signed char *) "reset_count", 512, NULL, 3, NULL);

    init_id();

    espconn_init();
    init_led();
    sync_init();
    i2c_master_gpio_init();
    dns_init();
    ntp_init();
    pcf8575_init();
    lock_init();
//    close_ad_lcd();
//    close_led();

    wifi_set_sleep_type(LIGHT_SLEEP_T);

//    xTaskCreate(&beep_test, (const signed char *) "beep_test", 512, NULL, 3, NULL);
//    xTaskCreate(&lock_test, (const signed char *) "lock_test", 512, NULL, 3, NULL);
//    xTaskCreate(&task_print_meminfo, (const signed char *) "meminfo", 512, NULL, 1, NULL);
    xTaskCreate(&task_smartconfig, (const signed char *) "smartconfig", 1024, NULL, 2, NULL);
    xTaskCreate(&task_kcp, (const signed char *) "kcp", 512, NULL, 1, NULL);
    xTaskCreate(&task_report, (const signed char *) "task_report", 512, NULL, 1, NULL);
    xTaskCreate(&task_kcp_recv, (const signed char *) "kcp_recv", 512, NULL, 1, NULL);
    xTaskCreate(&task_update_time, (const signed char *) "ds1307", 512, NULL, 3, NULL);

}
