//
// Created by Yan_X on 2022/4/5.
//

#include "ds1307.h"
#include "i2c.h"


uint8_t bcd2dec(uint8_t val) {
    return (val >> 4) * 10 + (val & 0x0f);
}


uint8_t dec2bcd(uint8_t val) {
    return ((val / 10) << 4) + (val % 10);
}


bool update_register(uint8_t reg, uint8_t mask, uint8_t val) {

    uint8_t old;
    i2c_master_readRegister(DS1307_ADDR, reg, &old);
    uint8_t buf = (old & mask) | val;
    return i2c_master_writeRegister(DS1307_ADDR, reg, buf);
}


bool ds1307_start(bool start) {
    return update_register(TIME_REG, CH_MASK, start ? 0 : CH_BIT);
}


bool ds1307_is_running() {
    uint8_t val;

    i2c_master_readRegister(DS1307_ADDR, TIME_REG, &val);

    return (val & CH_BIT) == 0;
}


bool ds1307_get_time(struct tm *time) {
    uint8_t buf[7];
    bool res;

    res = i2c_master_read_bytes(DS1307_ADDR, TIME_REG, buf, 7);

    time->tm_sec = bcd2dec(buf[0] & SECONDS_MASK);
    time->tm_min = bcd2dec(buf[1]);
    if (buf[2] & HOUR12_BIT) {
        // RTC in 12-hour mode
        time->tm_hour = bcd2dec(buf[2] & HOUR12_MASK) - 1;
        if (buf[2] & PM_BIT)
            time->tm_hour += 12;
    } else
        time->tm_hour = bcd2dec(buf[2] & HOUR24_MASK);
    time->tm_wday = bcd2dec(buf[3]) - 1;
    time->tm_mday = bcd2dec(buf[4]);
    time->tm_mon = bcd2dec(buf[5]) - 1;
    time->tm_year = bcd2dec(buf[6]) + 2000;

    return res;
}


bool ds1307_set_time(const struct tm *time) {
    uint8_t buf[7] = {
            dec2bcd(time->tm_sec),
            dec2bcd(time->tm_min),
            dec2bcd(time->tm_hour),
            dec2bcd(time->tm_wday + 1),
            dec2bcd(time->tm_mday),
            dec2bcd(time->tm_mon + 1),
            dec2bcd(time->tm_year - 2000)
    };

    return i2c_master_write_bytes(DS1307_ADDR, TIME_REG, buf, sizeof(buf));
}
