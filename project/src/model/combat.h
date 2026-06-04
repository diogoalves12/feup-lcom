#ifndef PROJECT_MODEL_COMBAT_H
#define PROJECT_MODEL_COMBAT_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "player.h"

#define COMBAT_DAMAGE 1
#define COMBAT_RAY_STEP 4
#define COMBAT_AIM_TOLERANCE 14
#define COMBAT_MAX_DISTANCE 900
#define COMBAT_SHOT_COOLDOWN_TICKS 20
#define COMBAT_SHOT_EFFECT_FRAMES 8

typedef struct {
  bool     active;
  Position start;
  Position end;
  float    angle;
  int      shooter_num;
  bool     hit;
  uint32_t start_frame;
  uint32_t expire_frame;
} ShotEffect;

typedef struct {
  uint32_t  player1_next_allowed_shot_frame;
  uint32_t  player2_next_allowed_shot_frame;
  ShotEffect last_shot;
} CombatState;

void combat_init(CombatState *combat);
void combat_reset(CombatState *combat);

bool combat_try_shoot(CombatState *combat, int shooter_num, const Player *shooter, Player *target, Arena *arena, uint32_t frame_counter);

#endif
