#ifndef MAIN_DS1307_H__
#define MAIN_DS1307_H__

#include "i2c.h"
#include <time.h>


#define DS1307_ADDR 0x68 //!< I2C address

#define RAM_SIZE 56

#define TIME_REG    0
#define CONTROL_REG 7
#define RAM_REG     8

#define CH_BIT      (1 << 7)
#define HOUR12_BIT  (1 << 6)
#define PM_BIT      (1 << 5)
#define SQWE_BIT    (1 << 4)
#define OUT_BIT     (1 << 7)

#define CH_MASK      0x7f
#define SECONDS_MASK 0x7f
#define HOUR12_MASK  0x1f
#define HOUR24_MASK  0x3f
#define SQWEF_MASK   0x03
#define SQWE_MASK    0xef
#define OUT_MASK     0x7

// bcd编码转十进制
uint8_t bcd2dec(uint8_t val);

// 十进制转bcd编码
uint8_t dec2bcd(uint8_t val);

bool ds1307_start(bool start);

bool ds1307_is_running();

bool ds1307_get_time(struct tm *time);

bool ds1307_set_time(const struct tm *time);


#endif /* MAIN_DS1307_H__ */
