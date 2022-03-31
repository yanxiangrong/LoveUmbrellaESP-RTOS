//
// Created by Yan_X on 2022/3/27.
//

#ifndef ESP_RTOS_NETWORK_H
#define ESP_RTOS_NETWORK_H

#include "cJSON.h"
#include "mtwist.h"

ikcpcb *mKCP;
struct espconn conn;
const char *server_host = "volunteer.fengxianhub.top";
const uint16_t server_port = 67;
char sendBuf[1024];
char recvBuf[1024];

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
    led_blink_once();
    result = espconn_sendto(&conn, (uint8 *) pBuf, lSize);
    if (result) {
        print_espconn_error(result);
    }

    return 0;
}

void udp_recv_callback(void *arg, char *pdata, unsigned short len) {
    printf("Received %d bytes UDP packet\n", len);
    led_blink_once();
    ikcp_input(mKCP, pdata, len);
}

LOCAL esp_udp conf_udp;

_Noreturn void task_kcp() {
    uint32_t conv;
    int result;
    bool res;
    ip_addr_t ipAddr;
    mtwist *mt;

    mt = mtwist_new();
    mtwist_init(mt, time_now());

    wait_time();
    wait_network();

    conv = mtwist_u32rand(mt);

    printf("KCP create, conv: %u\n", conv);

    res = hostnameToIp(server_host, &ipAddr);
    if (not res) {
        printf("ntp update time fail.\n");
        vTaskDelete(NULL);
    }

    memcpy(conf_udp.remote_ip, &ipAddr.addr, 4);
    printf("Server ip: %d.%d.%d.%d\n",
           conf_udp.remote_ip[0],
           conf_udp.remote_ip[1],
           conf_udp.remote_ip[2],
           conf_udp.remote_ip[3]);

    conf_udp.local_port = 49152 + (int) (conv % 16383);
    conf_udp.remote_port = server_port;
    conn.type = ESPCONN_UDP;
    conn.proto.udp = &(conf_udp);

    printf("UDP create, Local port: %d, remote port: %d\n", conf_udp.local_port, conf_udp.remote_port);
    result = (int) espconn_create(&conn);
    if (result) {
        print_espconn_error(result);
    }
    espconn_regist_recvcb(&conn, udp_recv_callback);

    mKCP = ikcp_create(conv, NULL);
    ikcp_nodelay(mKCP, 0, 40, 0, 0);
    ikcp_setmtu(mKCP, 512);
    ikcp_wndsize(mKCP, 16, 16);
    ikcp_setoutput(mKCP, udpSendOut);
    mKCP->rx_minrto = 100;

    printf("KCP Ok.\n");
    kcp_ok();

    while (true) {
        ikcp_update(mKCP, system_get_time());
        taskYIELD();
    }

    vTaskDelete(NULL);
}

_Noreturn void task_kcp_recv() {
    int hr;

    wait_kcp();

    while (true) {
        hr = ikcp_recv(mKCP, recvBuf, sizeof(recvBuf));
        if (hr < 0) {
            vTaskDelay(0);
            continue;
        }
        printf("[KCP] Received:  %s\n", recvBuf);
        recvBuf[hr] = 0;
    }
    vTaskDelete(NULL);
}

cJSON *status_json() {
    cJSON *root = cJSON_CreateObject();
    cJSON *status = cJSON_CreateObject();


    cJSON_AddItemToObject(status, "cpu_freq", cJSON_CreateNumber(system_get_cpu_freq()));
    cJSON_AddItemToObject(status, "sdk_version", cJSON_CreateString(system_get_sdk_version()));
    cJSON_AddItemToObject(status, "free_heap", cJSON_CreateNumber(system_get_free_heap_size()));
    cJSON_AddItemToObject(status, "work_time", cJSON_CreateNumber(system_get_time()));
    cJSON_AddItemToObject(status, "chip_id", cJSON_CreateNumber(system_get_chip_id()));

    cJSON_AddItemToObject(root, "id", cJSON_CreateNumber(system_get_chip_id()));
    cJSON_AddItemToObject(root, "post_type", cJSON_CreateString("status"));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(int64_to_str(time_now() / 1000)));
    cJSON_AddItemToObject(root, "status", status);
    return root;
}

_Noreturn void task_report() {
    wait_kcp();

    while (true) {
        if (ikcp_waitsnd(mKCP) == 0) {
            cJSON *jsonObj = status_json();
            char *jsonStr = cJSON_PrintUnformatted(jsonObj);
            cJSON_Minify(jsonStr);

            strcpy(sendBuf, jsonStr);
            free(jsonStr);

            ikcp_send(mKCP, sendBuf, (int) strlen(sendBuf));
            printf("[KCP] Sent: %s\n", sendBuf);
            cJSON_Delete(jsonObj);
            vTaskDelay(5000 / portTICK_RATE_MS);
        }
    }

    vTaskDelete(NULL);
}


#endif //ESP_RTOS_NETWORK_H
