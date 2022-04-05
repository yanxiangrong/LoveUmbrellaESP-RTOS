//
// Created by Yan_X on 2022/3/21.
//

#ifndef ESP_RTOS_WIFI_H
#define ESP_RTOS_WIFI_H


#include "esp_common.h"

#include "espressif/espconn.h"
#include "espressif/airkiss.h"

#include "leds.h"
#include "uitls.h"
#include "sync.h"
#include "user_config.h"


#define DEVICE_TYPE         "gh_9e2cff3dfa51"  //wechat public number

#define DEFAULT_LAN_PORT    12476


LOCAL esp_udp ssdp_udp;
LOCAL struct espconn pssdpudpconn;
LOCAL os_timer_t ssdp_time_serv;

uint8 lan_buf[200];
uint16 lan_buf_len;
uint8 udp_sent_cnt = 0;

const airkiss_config_t akconf =
        {
                (airkiss_memset_fn) &memset,
                (airkiss_memcpy_fn) &memcpy,
                (airkiss_memcmp_fn) &memcmp,
                0,
        };


LOCAL void ICACHE_FLASH_ATTR
airkiss_wifilan_time_callback(void) {
    uint16 i;
    airkiss_lan_ret_t ret;

    if ((udp_sent_cnt++) > 30) {
        udp_sent_cnt = 0;
        os_timer_disarm(&ssdp_time_serv);//s
        //return;
    }

    ssdp_udp.remote_port = DEFAULT_LAN_PORT;
    ssdp_udp.remote_ip[0] = 255;
    ssdp_udp.remote_ip[1] = 255;
    ssdp_udp.remote_ip[2] = 255;
    ssdp_udp.remote_ip[3] = 255;
    lan_buf_len = sizeof(lan_buf);
    ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD,
                           DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);
    if (ret != AIRKISS_LAN_PAKE_READY) {
        os_printf("Pack lan packet error!");
        return;
    }

    ret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
    if (ret != 0) {
        os_printf("UDP send error!");
    }
    os_printf("Finish send notify!\n");
}

LOCAL void ICACHE_FLASH_ATTR
airkiss_wifilan_recv_callbk(void *arg, char *pdata, unsigned short len) {
    uint16 i;
    remot_info *pcon_info = NULL;

    airkiss_lan_ret_t ret = airkiss_lan_recv(pdata, len, &akconf);
    airkiss_lan_ret_t packret;

    switch (ret) {
        case AIRKISS_LAN_SSDP_REQ:
            espconn_get_connection_info(&pssdpudpconn, &pcon_info, 0);
            os_printf("remote ip: %d.%d.%d.%d \r\n", pcon_info->remote_ip[0], pcon_info->remote_ip[1],
                      pcon_info->remote_ip[2], pcon_info->remote_ip[3]);
            os_printf("remote port: %d \r\n", pcon_info->remote_port);

            pssdpudpconn.proto.udp->remote_port = pcon_info->remote_port;
            memcpy(pssdpudpconn.proto.udp->remote_ip, pcon_info->remote_ip, 4);
            ssdp_udp.remote_port = DEFAULT_LAN_PORT;

            lan_buf_len = sizeof(lan_buf);
            packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,
                                       DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);

            if (packret != AIRKISS_LAN_PAKE_READY) {
                os_printf("Pack lan packet error!");
                return;
            }

            os_printf("\r\n\r\n");
            for (i = 0; i < lan_buf_len; i++)
                os_printf("%c", lan_buf[i]);
            os_printf("\r\n\r\n");

            packret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
            if (packret != 0) {
                os_printf("LAN UDP Send err!");
            }

            break;
        default:
            os_printf("Pack is not ssdq req!%d\r\n", ret);
            break;
    }
}

void ICACHE_FLASH_ATTR
airkiss_start_discover(void) {
    ssdp_udp.local_port = DEFAULT_LAN_PORT;
    pssdpudpconn.type = ESPCONN_UDP;
    pssdpudpconn.proto.udp = &(ssdp_udp);
    espconn_regist_recvcb(&pssdpudpconn, airkiss_wifilan_recv_callbk);
    espconn_create(&pssdpudpconn);

    os_timer_disarm(&ssdp_time_serv);
    os_timer_setfn(&ssdp_time_serv, (os_timer_func_t *) airkiss_wifilan_time_callback, NULL);
    os_timer_arm(&ssdp_time_serv, 1000, 1);//1s
}

void smartconfig_callback(sc_status status, void *pdata) {
    switch (status) {
        case SC_STATUS_WAIT:
            printf("SC_STATUS_WAIT\n");
            break;

        case SC_STATUS_FIND_CHANNEL:
            printf("SC_STATUS_FIND_CHANNEL\n");
            break;

        case SC_STATUS_GETTING_SSID_PSWD:
            if (*(sc_type *) pdata == SC_TYPE_ESPTOUCH) {
                printf("Getting ssid and passwd by ESP-TOUCH.\n");
                break;
            }
            if (*(sc_type *) pdata == SC_TYPE_AIRKISS) {
                printf("Getting ssid and passwd by AIR KISS.\n");
                break;
            }

        case SC_STATUS_LINK:
            printf("Link to wifi. ssid: %s, password: %s, mac of ap: %s\n",
                   ((struct station_config *) pdata)->ssid,
                   ((struct station_config *) pdata)->password,
                   ((struct station_config *) pdata)->bssid
            );

            led_always_on();

            struct station_config *wifi_conf = pdata;
            wifi_station_disconnect();
            wifi_station_set_config(wifi_conf);
            wifi_station_connect();

            break;

        case SC_STATUS_LINK_OVER:
            printf("Link over.\n");
            if (pdata) {
                printf("By mobile phone: %d.%d.%d.%d",
                       ((int *) pdata)[0],
                       ((int *) pdata)[1],
                       ((int *) pdata)[2],
                       ((int *) pdata)[3]
                );
            } else {
                //SC_TYPE_AIRKISS - support airkiss v2.0
                airkiss_start_discover();
            }
            smartconfig_stop();
            led_off();
            break;

        default:
            break;
    }
}


void task_smartconfig(void *ignore) {
    bool result = false;
    struct station_config sta_conf;

    char test_ssid[] = "IOT_2508";
    char test_password[] = "wlwsys6688";

    wifi_set_opmode(STATION_MODE);

    wifi_station_get_config_default(&sta_conf);

    memcpy(&sta_conf.ssid, test_ssid, sizeof (test_ssid));
    memcpy(&sta_conf.password, test_password, sizeof (test_password));
    wifi_station_set_config(&sta_conf);

    led_always_on();

    if (strlen((const char *) sta_conf.ssid) == 0) {
        smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS);
        result = smartconfig_start(smartconfig_callback, 1);
        printf("Smartconfig start %s.\n", result ? "success" : "fail");
        led_blink_fast();
    }

    while (true) {
        if (wifi_station_get_connect_status() == STATION_GOT_IP)    // 等待连接到 AP
        {
            break;
        }
        vTaskDelay(200 / portTICK_RATE_MS);
    }
    led_off();

    printf("Network Ok.\n");
    network_ok();
    vTaskDelete(NULL);
}


#endif //ESP_RTOS_WIFI_H
