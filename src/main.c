#include "esp_common.h"

#include "ikcp.h"
#include "leds.h"
#include "wifi.h"
#include "restore.h"


_Noreturn void task_print_meminfo(void *ignore) {
    portTickType xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    while (true) {
        system_print_meminfo();
        printf("Free heap size: %d\n\n", system_get_free_heap_size());
        vTaskDelayUntil(&xLastWakeTime, 10000 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

ikcpcb *mKCP;
struct espconn conn;
const char *host = "volunteer.fengxianhub.top";
const uint16_t port = 67;

void print_espconn_error(int code) {
    switch (code) {
        case ESPCONN_MEM:
            printf("Out of memory!\n");
            break;
        case ESPCONN_MAXNUM:
            printf("Buffer of sending data is full!\n");
            break;
        case ESPCONN_ARG:
            printf("Illegal argument!\n");
            break;
        case ESPCONN_IF:
            printf("Send UDP data fail!\n");
            break;
        default:
            break;
    }
}

int udpSendOut(const char *pBuf, int lSize, ikcpcb *pKCP, void *pCTX) {
    int result;

    printf("Send %d bytes UDP packet\n", lSize);
    result = espconn_sendto(&conn, (uint8 *) pBuf, lSize);
    if (result) {
        print_espconn_error(result);
    }

    return 0;
}

_Noreturn void task_kcp() {
    uint32_t conv;
    int result;
    conv = rand();

    ssdp_udp.remote_ip[0] = 47;
    ssdp_udp.remote_ip[1] = 101;
    ssdp_udp.remote_ip[2] = 223;
    ssdp_udp.remote_ip[3] = 63;
    ssdp_udp.local_port = 1024 + rand() % 3976;
    ssdp_udp.remote_port = port;
    pssdpudpconn.type = ESPCONN_UDP;
    pssdpudpconn.proto.udp = &(ssdp_udp);

    printf("KCP create, conv: %d", conv);
    mKCP = ikcp_create(conv, NULL);
    ikcp_nodelay(mKCP, 0, 40, 0, 0);
    ikcp_wndsize(mKCP, 16, 16);
    mKCP->rx_minrto = 100;
    mKCP->output = udpSendOut;

    result = espconn_create(&conn);
    if (result) {
        print_espconn_error(result);
    }

    while (true) {
        ikcp_update(mKCP, system_get_time());
        taskYIELD();
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
    printf("SDK version:%s\n", system_get_sdk_version());

    espconn_init();
    init_led();

    wifi_set_sleep_type(LIGHT_SLEEP_T);

    xTaskCreate(&task_smartconfig, (const signed char *) "smartconfig", 1024, NULL, 2, NULL);
//    xTaskCreate(&task_print_meminfo, (const signed char *) "meminfo", 512, NULL, 1, NULL);
//    xTaskCreate(&task_kcp, (const signed char *) "kcp", 512, NULL, 1, NULL);
    xTaskCreate(&task_reset_count, (const signed char *) "reset_count", 512, NULL, 3, NULL);
}
