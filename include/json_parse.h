//
// Created by Yan_X on 2022/4/3.
//

#ifndef ESP_RTOS_JSON_PARSE_H
#define ESP_RTOS_JSON_PARSE_H


#define MAX_TOLERANCE 10000

#define check_type(object, d_type)    \
if(object->type != d_type) {          \
    printf("json parse error!\n");    \
    break;                            \
}

#define CODE_LED 2002
#define CODE_LCD 2003
#define CODE_LOCK 2004

void p21arse(const char *json_str);

#endif //ESP_RTOS_JSON_PARSE_H
