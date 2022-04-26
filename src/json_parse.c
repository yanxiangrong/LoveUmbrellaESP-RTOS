//
// Created by Yan_X on 2022/4/5.
//

#include "json_parse.h"
#include "esp_common.h"
#include "cJSON.h"
#include "user_config.h"
#include "uitls.h"
#include "my_devices.h"
#include "network.h"
#include "sync.h"
#include "events.h"


void str_fill_zero(char *string, int n) {
    int len;
    len = (int) strlen(string);
    for (int i = len; i >= 0; --i) {
        string[i + n] = string[i];
    }
    for (int i = 0; i < n; ++i) {
        string[i] = '0';
    }
}


int diff_timestamp(char *timestamp) {
    char *sys_timestamp;
    int len_diff;
    int a, b;

    sys_timestamp = int64_to_str(time_now() / 1000);

    len_diff = (int) (strlen(timestamp) - strlen(sys_timestamp));
    if (len_diff > 0) {
        str_fill_zero(sys_timestamp, len_diff);
    } else {
        str_fill_zero(timestamp, len_diff);
    }

    for (int i = 0; i < len_diff - 4; ++i) {
        if (timestamp[i] == sys_timestamp[i]) {
            timestamp[i] = ' ';
            sys_timestamp[i] = ' ';
        } else {
            return 1000;
        }
    }

    sscanf(timestamp, "%d", &a);
    sscanf(sys_timestamp, "%d", &b);

    return abs(a - b);
}


void return_json(cJSON *json) {
    cJSON *content = cJSON_CreateObject();

    cJSON_AddNumberToObject(content, "code", 100);
    cJSON_AddStringToObject(content, "message", "ok");

    cJSON_AddItemToObject(json, "post_type", cJSON_CreateString("return"));
    cJSON_AddItemToObject(json, "content", content);
}

void event_json(cJSON *json, int event) {
    cJSON *content = cJSON_CreateObject();

    cJSON_AddNumberToObject(content, "code", event);
    cJSON_AddStringToObject(content, "message", event_string(event));

    cJSON_AddItemToObject(json, "post_type", cJSON_CreateString("event"));
    cJSON_AddItemToObject(json, "content", content);
}

void led_control(cJSON *json) {
    cJSON *power = cJSON_GetObjectItem(json, "power");

    switch (power->valueint) {
        case true:
            open_led();
            break;
        case false:
            close_led();
            break;
    }
}


void lcd_control(cJSON *json) {
    cJSON *power = cJSON_GetObjectItem(json, "power");

    switch (power->valueint) {
        case true:
            open_ad_lcd();
            break;
        case false:
            close_ad_lcd();
            break;
    }
}


void lock_control(cJSON *json) {
    cJSON *power = cJSON_GetObjectItem(json, "lock_switch");

    switch (power->valueint) {
        case true:
            open_lock();
            wait_detect();
            close_lock();
            break;
        case false:
            close_lock();
            break;
    }
}


void parse_control(cJSON *json) {
    cJSON *target = cJSON_GetObjectItem(json, "target");
    cJSON *operation = cJSON_GetObjectItem(json, "operation");
    cJSON *device_code = cJSON_GetObjectItem(target, "device_code");
    cJSON *ret, *event;

    switch (device_code->valueint) {
        case CODE_LED:
            led_control(operation);
            break;
        case CODE_LCD:
            lcd_control(operation);
            break;
        case CODE_LOCK:
            ret = comm_json();
            event = comm_json();
            return_json(ret);
            send_json(ret);

            lock_control(operation);
            event_json(event, 3001);
            send_json(event);

            cJSON_Delete(ret);
            cJSON_Delete(event);

            break;
    }
}


void parse_config_wifi(cJSON *item) {
    struct station_config config = {};

    wifi_station_get_config(&config);

    cJSON *ssid = cJSON_GetObjectItem(item, "ssid");
    cJSON *passwd = cJSON_GetObjectItem(item, "password");
    if (ssid != NULL) {
        strcpy((char *) config.ssid, ssid->valuestring);
    }

    if (passwd != NULL) {
        strcpy((char *) config.password, passwd->valuestring);
    }

    wifi_station_set_config(&config);
}


void parse_config(cJSON *json) {
    cJSON *config_item_list = cJSON_GetObjectItem(json, "config_items");
    cJSON *item;

    for (int i = 0; i < cJSON_GetArraySize(config_item_list); ++i) {
        item = cJSON_GetArrayItem(config_item_list, i);
        if (strcmp(item->string, "wifi") == 0) {
            parse_config_wifi(item);
        }
    }
}


void parse(const char *json_str) {
    cJSON *json = cJSON_Parse(json_str);
    cJSON *id = cJSON_GetObjectItem(json, "id");
    cJSON *post_type = cJSON_GetObjectItem(json, "post_type");
    cJSON *timestamp = cJSON_GetObjectItem(json, "timestamp");
    cJSON *content = cJSON_GetObjectItem(json, "content");

    do {
        check_type(id, cJSON_Number)
        if (id->valueint != get_device_id_int()) {
            break;
        }

        check_type(timestamp, cJSON_String)
        if (diff_timestamp(timestamp->valuestring) > MAX_TOLERANCE) {
            printf("json expired!\n");
            break;
        }

        check_type(post_type, cJSON_String)
        if (strcmp(post_type->valuestring, "control") == 0) {
            parse_control(content);
            break;
        }
        if (strcmp(post_type->valuestring, "config") == 0) {
            parse_config(content);
            break;
        }

    } while (0);


    cJSON_Delete(json);
}
