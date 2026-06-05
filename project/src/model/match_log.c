#include "match_log.h"

#include <stddef.h>

void match_log_init(MatchLog *log) {
  if (log == NULL) return;
  log->count = 0;
  for (int i = 0; i < MAX_MATCH_LOGS; i++) {
    log->entries[i].valid = false;
    log->entries[i].valid_timestamp = false;
    log->entries[i].winner = 0;
    log->entries[i].difficulty = ARENA_EASY;
  }
}

void match_log_add(MatchLog *log, const MatchLogEntry *entry) {
  if (log == NULL || entry == NULL) return;

  if (log->count < MAX_MATCH_LOGS) {
    log->entries[log->count] = *entry;
    log->entries[log->count].valid = true;
    log->count++;
    return;
  }

  for (int i = 1; i < MAX_MATCH_LOGS; i++) {
    log->entries[i - 1] = log->entries[i];
  }
  log->entries[MAX_MATCH_LOGS - 1] = *entry;
  log->entries[MAX_MATCH_LOGS - 1].valid = true;
}
