#ifndef PROJECT_MODEL_MATCH_LOG_H
#define PROJECT_MODEL_MATCH_LOG_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "rtc.h"

#define MAX_MATCH_LOGS 5

typedef struct {
  RtcDateTime     timestamp;
  int             winner;
  ArenaDifficulty difficulty;
  bool            valid_timestamp;
  bool            valid;
} MatchLogEntry;

typedef struct {
  MatchLogEntry entries[MAX_MATCH_LOGS];
  uint8_t       count;
} MatchLog;

void match_log_init(MatchLog *log);
void match_log_add(MatchLog *log, const MatchLogEntry *entry);

#endif
