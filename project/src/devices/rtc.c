#include "rtc.h"

#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef OK
#define OK 0
#endif

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

#define RTC_REG_SECONDS 0x00
#define RTC_REG_MINUTES 0x02
#define RTC_REG_HOURS   0x04
#define RTC_REG_DAY     0x07
#define RTC_REG_MONTH   0x08
#define RTC_REG_YEAR    0x09
#define RTC_REG_A       0x0A
#define RTC_REG_B       0x0B

#define RTC_A_UIP   (1 << 7)
#define RTC_B_24H   (1 << 1)
#define RTC_B_DM    (1 << 2)
#define RTC_HOUR_PM (1 << 7)

#define RTC_UIP_MAX_RETRIES 10

static uint8_t bcd_to_bin(uint8_t value) {
  return (uint8_t)(((value >> 4) * 10) + (value & 0x0F));
}

static int rtc_read_reg(uint8_t reg, uint8_t *value) {
  uint32_t tmp;
  if (sys_outb(RTC_ADDR_REG, reg) != OK) return 1;
  if (sys_inb(RTC_DATA_REG, &tmp) != OK) return 1;
  *value = (uint8_t)tmp;
  return 0;
}

static int rtc_wait_not_updating(void) {
  uint8_t reg_a;
  for (int i = 0; i < RTC_UIP_MAX_RETRIES; i++) {
    if (rtc_read_reg(RTC_REG_A, &reg_a) != 0) return 1;
    if ((reg_a & RTC_A_UIP) == 0) return 0;
    tickdelay(micros_to_ticks(1000));
  }
  return 1;
}

int rtc_read_datetime(RtcDateTime *dt) {
  if (dt == NULL) return 1;

  if (rtc_wait_not_updating() != 0) return 1;

  uint8_t reg_b;
  if (rtc_read_reg(RTC_REG_B, &reg_b) != 0) return 1;
  bool binary_mode = (reg_b & RTC_B_DM) != 0;
  bool hour_24_mode = (reg_b & RTC_B_24H) != 0;

  uint8_t sec, min, hour, day, mon, year;
  if (rtc_read_reg(RTC_REG_SECONDS, &sec) != 0) return 1;
  if (rtc_read_reg(RTC_REG_MINUTES, &min) != 0) return 1;
  if (rtc_read_reg(RTC_REG_HOURS,   &hour) != 0) return 1;
  if (rtc_read_reg(RTC_REG_DAY,     &day) != 0) return 1;
  if (rtc_read_reg(RTC_REG_MONTH,   &mon) != 0) return 1;
  if (rtc_read_reg(RTC_REG_YEAR,    &year) != 0) return 1;

  bool hour_pm = false;
  if (!hour_24_mode) {
    hour_pm = (hour & RTC_HOUR_PM) != 0;
    hour &= (uint8_t)~RTC_HOUR_PM;
  }

  if (!binary_mode) {
    sec  = bcd_to_bin(sec);
    min  = bcd_to_bin(min);
    hour = bcd_to_bin(hour);
    day  = bcd_to_bin(day);
    mon  = bcd_to_bin(mon);
    year = bcd_to_bin(year);
  }

  if (!hour_24_mode) {
    if (hour == 12) hour = 0;
    if (hour_pm) hour = (uint8_t)(hour + 12);
  }

  dt->second = sec;
  dt->minute = min;
  dt->hour   = hour;
  dt->day    = day;
  dt->month  = mon;
  dt->year   = (uint16_t)(2000 + year);

  return 0;
}
