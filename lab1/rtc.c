#include "rtc.h"

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include <stdbool.h>

#ifndef OK
#define OK 0
#endif

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK (1 << 7)
#define RTC_DM_MSK (1 << 2)

static int bcd_to_bin(uint8_t bcd) {
  return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static int rtc_read_reg(uint8_t reg, uint8_t *value) {
  uint32_t tmp;
  if (sys_outb(RTC_ADDR_REG, reg) != OK) return -1;
  if (sys_inb(RTC_DATA_REG, &tmp) != OK) return -1;
  *value = (uint8_t)tmp;
  return 0;
}

int rtc_read_date(rtc_date *date) {
  if (date == NULL) return -1;

  uint8_t reg_a;
  // wait for UIP to clear to ensure consistent reads
  do {
    if (rtc_read_reg(RTC_REG_A, &reg_a)) return -1;
    if (reg_a & RTC_UIP_MSK) tickdelay(micros_to_ticks(244));
  } while (reg_a & RTC_UIP_MSK);

  uint8_t reg_b;
  if (rtc_read_reg(RTC_REG_B, &reg_b)) return -1;
  bool binary_mode = reg_b & RTC_DM_MSK;

  uint8_t day_raw, month_raw, year_raw;
  if (rtc_read_reg(RTC_REG_DAY, &day_raw)) return -1;
  if (rtc_read_reg(RTC_REG_MONTH, &month_raw)) return -1;
  if (rtc_read_reg(RTC_REG_YEAR, &year_raw)) return -1;

  if (!binary_mode) {
    day_raw = (uint8_t)bcd_to_bin(day_raw);
    month_raw = (uint8_t)bcd_to_bin(month_raw);
    year_raw = (uint8_t)bcd_to_bin(year_raw);
  }

  date->day = day_raw;
  date->month = month_raw;
  date->year = year_raw;

  return 0;
}
