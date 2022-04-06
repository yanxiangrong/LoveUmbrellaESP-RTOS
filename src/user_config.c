//
// Created by Yan_X on 2022/4/5.
//

#include "user_config.h"

char DEVICE_ID[20] = {};
int32_t DEVICE_ID_INT = 0;

void init_id() {
    DEVICE_ID_INT = (int32_t) (system_get_chip_id() & ~(1 << 31));
    sprintf(DEVICE_ID, "%u", DEVICE_ID_INT);
}

char *get_device_id() {
    return DEVICE_ID;
}


int32_t get_device_id_int() {
    return DEVICE_ID_INT;
}