#ifndef PROJECT_DEVICES_RTC_H
#define PROJECT_DEVICES_RTC_H

#include <stdint.h>

typedef struct {
  uint8_t  second;
  uint8_t  minute;
  uint8_t  hour;
  uint8_t  day;
  uint8_t  month;
  uint16_t year;
} RtcDateTime;

int rtc_read_datetime(RtcDateTime *dt);

#endif
