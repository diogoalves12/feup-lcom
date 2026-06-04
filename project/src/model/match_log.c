#include "match_log.h"
#include <stddef.h>

void match_log_init(MatchLog *log) {
  if (log == NULL) return;
  log->count = 0;
}

void match_log_add(MatchLog *log, int winner) {
  if (log == NULL) return;

  MatchLogEntry entry;
  entry.winner = winner;

  if (rtc_read_date(&entry.date) != 0) {
    entry.date.day = 0;
    entry.date.month = 0;
    entry.date.year = 0;
  }

  if (rtc_read_time(&entry.time) != 0) {
    entry.time.hours = 0;
    entry.time.minutes = 0;
    entry.time.seconds = 0;
  }

  if (log->count < MAX_MATCH_LOGS) {
    log->entries[log->count] = entry;
    log->count++;
  } else {
    for (int i = 1; i < MAX_MATCH_LOGS; i++) {
      log->entries[i - 1] = log->entries[i];
    }
    log->entries[MAX_MATCH_LOGS - 1] = entry;
  }
}
