//
// Created by Yan_X on 2022/4/10.
//

#ifndef ESP_RTOS_EVENTS_H
#define ESP_RTOS_EVENTS_H

#define EVENT_DETECTED 3001
#define EVENT_DETECTED_STRING "An umbrella was detected"

const char *event_string(int event) {
    switch (event) {
        case EVENT_DETECTED:
            return EVENT_DETECTED_STRING;
    }

    return "";
}

#endif //ESP_RTOS_EVENTS_H
