#ifndef PROJECT_MODEL_MATCH_LOG_H
#define PROJECT_MODEL_MATCH_LOG_H

#include "../../lab1/rtc.h"
#include <stdint.h>

#define MAX_MATCH_LOGS 10

typedef struct {
  rtc_date date;
  rtc_time time;
  int winner; // 1 or 2
} MatchLogEntry;

typedef struct {
  MatchLogEntry entries[MAX_MATCH_LOGS];
  int count;
} MatchLog;

void match_log_init(MatchLog *log);
void match_log_add(MatchLog *log, int winner);

#endif
