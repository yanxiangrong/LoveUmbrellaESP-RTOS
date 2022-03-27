//
// Created by Yan_X on 2022/3/27.
//

#ifndef ESP_RTOS_NETWORK_H
#define ESP_RTOS_NETWORK_H

ikcpcb *mKCP;
struct espconn conn;
const char *host = "volunteer.fengxianhub.top";
const uint16_t port = 67;
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
    result = espconn_sendto(&conn, (uint8 *) pBuf, lSize);
    if (result) {
        print_espconn_error(result);
    }

    return 0;
}

void udp_recv_callback(void *arg, char *pdata, unsigned short len) {
    ikcp_input(mKCP, pdata, len);
    printf("Received %d bytes UDP packet\n", len);
}

LOCAL esp_udp conf_udp;

_Noreturn void task_kcp() {
    uint32_t conv;
    int result;

    wait_network();


    conv = rand();

    printf("KCP create, conv: %d\n", conv);

    conf_udp.remote_ip[0] = 47;
    conf_udp.remote_ip[1] = 101;
    conf_udp.remote_ip[2] = 223;
    conf_udp.remote_ip[3] = 63;
    conf_udp.local_port = 1024 + rand() % 3976;
    conf_udp.remote_port = port;
    conn.type = ESPCONN_UDP;
    conn.proto.udp = &(conf_udp);

    result = espconn_create(&conn);
    if (result) {
        print_espconn_error(result);
    }
    espconn_regist_recvcb(&conn, udp_recv_callback);

    mKCP = ikcp_create(conv, NULL);
    ikcp_nodelay(mKCP, 0, 40, 0, 0);
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

_Noreturn void task_report() {
    wait_kcp();

    while (true) {
        if (ikcp_waitsnd(mKCP) == 0) {
            strcpy(sendBuf, "Hwcping00T");
            ikcp_send(mKCP, sendBuf, (int) strlen(sendBuf));
            printf("[KCP] Sent: %s\n", sendBuf);
            vTaskDelay(5000 / portTICK_RATE_MS);
        }
    }

    vTaskDelete(NULL);
}


#endif //ESP_RTOS_NETWORK_H
