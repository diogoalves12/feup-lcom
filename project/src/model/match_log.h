/**
 * @file match_log.h
 * @brief Recent match results kept in memory.
 *
 * Each entry stores winner, difficulty and an optional RTC timestamp.
 * The log screen displays the entries collected during this session.
 */
#ifndef MATCH_LOG_H
#define MATCH_LOG_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "rtc.h"

/** @brief Maximum number of match results kept in one session. */
#define MAX_MATCH_LOGS 5

/**
 * @brief One completed match.
 */
typedef struct {
  RtcDateTime timestamp;       /**< Date and time when the match ended. */
  int winner;                  /**< Winning player number (1 or 2). */
  ArenaDifficulty difficulty;  /**< Difficulty of the completed match. */
  bool valid_timestamp;       /**< False if the RTC read failed. */
  bool valid;                 /**< False for unused slots in the log. */
} MatchLogEntry;

/**
 * @brief Fixed size list of recent match results.
 */
typedef struct {
  MatchLogEntry entries[MAX_MATCH_LOGS];  /**< Entry pool. */
  uint8_t count;                          /**< Number of valid entries currently stored. */
} MatchLog;

/**
 * @brief Clears all entries.
 */
void match_log_init(MatchLog *log);

/**
 * @brief Adds one entry, dropping the oldest when full.
 */
void match_log_add(MatchLog *log, const MatchLogEntry *entry);

#endif
