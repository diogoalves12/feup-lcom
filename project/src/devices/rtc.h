/**
 * @file rtc.h
 * @brief RTC timestamp reading.
 *
 * Used to timestamp match log entries. Values are read from the CMOS RTC and
 * converted from BCD when needed.
 */
#ifndef RTC_H
#define RTC_H

#include <stdint.h>

/**
 * @brief Decoded date and time.
 */
typedef struct {
  uint8_t  second; /**< Seconds (0-59). */
  uint8_t  minute; /**< Minutes (0-59). */
  uint8_t  hour;   /**< Hours (0-23). */
  uint8_t  day;    /**< Day of the month (1-31). */
  uint8_t  month;  /**< Month (1-12). */
  uint16_t year;   /**< Full four-digit year. */
} RtcDateTime;

/**
 * @brief Reads the current date and time from the CMOS RTC.
 *
 * Waits until the RTC is not updating before reading the registers.
 */
int rtc_read_datetime(RtcDateTime *dt);

#endif
