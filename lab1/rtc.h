#pragma once

#include <stdint.h>

typedef struct {
  uint8_t day;
  uint8_t month;
  uint8_t year;
} rtc_date;

typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} rtc_time;

/**
 * Reads the current date from the RTC and fills the provided `rtc_date`
 * structure. Returns 0 on success, non-zero on failure.
 *
 * This function takes care of ensuring data consistency by checking the Update
 * In Progress (UIP) flag before each read. It also should check the RTC
 * configuration and perform data conversions if necessary (e.g. BCD to binary).
 */
int rtc_read_date(rtc_date *date);

/**
 * Reads the current time from the RTC and fills the provided `rtc_time`
 * structure. Returns 0 on success, non-zero on failure.
 */
int rtc_read_time(rtc_time *time);
