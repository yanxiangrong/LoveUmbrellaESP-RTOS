//
// Created by Yan_X on 2022/4/5.
//

#include "lwip/ip4_addr.h"
#include "ntp.h"
#include "esp_common.h"
#include "freertos/semphr.h"
#include "uitls.h"
#include "leds.h"
#include "ds1307.h"
#include "lwip/udp.h"

#define NTP_SERVER "ntp.aliyun.com"


static struct udp_pcb *ntp_pcb;
LOCAL xSemaphoreHandle responseMutex;


typedef struct {

    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
    // li.   Two bits.   Leap indicator.
    // vn.   Three bits. Version number of the protocol.
    // mode. Three bits. Client will pick mode 3 for client.

    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

} ntp_packet;              // Total: 384 bits or 48 bytes.


void ntp_send(ip_addr_t *);

void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port);

void ntp_init() {
    vSemaphoreCreateBinary(responseMutex)

    ntp_pcb = udp_new();

    if (ntp_pcb != NULL) {
        /* initialize DNS client */
        udp_bind(ntp_pcb, IP_ADDR_ANY, 0);
        udp_recv(ntp_pcb, ntp_recv, NULL);

    }
}

void ntp_run() {
    ip_addr_t ipAddr;
    bool res;
    int delay, failCount;

    res = hostnameToIp(NTP_SERVER, &ipAddr);
    if (not res) {
        printf("ntp update time fail.\n");
        return;
    }

    delay = 500;
    failCount = 0;

    printf("NTP server ip: %s\n", ipaddr_ntoa(&ipAddr));

    xSemaphoreTake(responseMutex, portMAX_DELAY);
    ntp_send(&ipAddr);
    while (xSemaphoreTake(responseMutex, delay / portTICK_RATE_MS) == pdFALSE) {
        ntp_send(&ipAddr);
        failCount++;
        delay += delay / 2;
    }
}

void ntp_send(ip_addr_t *ntpAddr) {
    ntp_packet packet = {};
    struct pbuf *buf;
    // Set the first byte's bits to 11,011,011 for li = 3, vn = 3, and mode = 3. The rest will be left set to zero.
    packet.li_vn_mode = 0xDB;
    packet.poll = 17;
    packet.txTm_s = htonl((time_now() + NTP_MICROS_DELTA) / 1000000);
    packet.txTm_f = htonl(time_now() % 1000000);

    buf = pbuf_alloc(PBUF_TRANSPORT, sizeof(ntp_packet), PBUF_RAM);

    pbuf_take(buf, &packet, sizeof(ntp_packet));

    udp_connect(ntp_pcb, ntpAddr, NTP_SERVER_PORT);

    printf("Send ntp request.\n");
    led_blink_once();
    udp_sendto(ntp_pcb, buf, ntpAddr, NTP_SERVER_PORT);

    udp_disconnect(ntp_pcb);
    pbuf_free(buf);
}

void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port) {
    ntp_packet packet = {};
    int64_t origTm, refTm, txTm, endTm, nowTm, diff;
    struct tm tm1;

    xSemaphoreGive(responseMutex);

    led_blink_once();
    pbuf_copy_partial(p, &packet, sizeof(packet), 0);

    endTm = time_now() + NTP_MICROS_DELTA;
    origTm = ((int64_t) ntohl(packet.origTm_s)) * 1000000 + ((int64_t) (ntohl(packet.txTm_f) / 4295));
    refTm = ((int64_t) ntohl(packet.refTm_s)) * 1000000 + ((int64_t) (ntohl(packet.refTm_f) / 4295));
    txTm = ((int64_t) ntohl(packet.txTm_s)) * 1000000 + ((int64_t) (ntohl(packet.txTm_f) / 4295));

    diff = (refTm >> 1) - (origTm >> 1) + (txTm >> 1) - (endTm >> 1);

    nowTm = time_now() + diff;
    update_time(nowTm);

    if (abs(diff) > 1000000) {
        timestampToTm((time_t) (nowTm / 1000000), &tm1);
        printf("Set ds1037 time.\n");
        ds1307_set_time(&tm1);
    }

    time_t tm = (time_t) (time_now() / 1000000);
    printf("Time updated, now time: UTC %s", ctime(&tm));
    pbuf_free(p);
}
