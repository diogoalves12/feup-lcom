#ifndef PROJECT_MODEL_COMBAT_H
#define PROJECT_MODEL_COMBAT_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "player.h"

#define COMBAT_DAMAGE 1
#define COMBAT_RAY_STEP 4
#define COMBAT_AIM_TOLERANCE 12
#define COMBAT_MAX_DISTANCE 900
#define COMBAT_SHOT_COOLDOWN_TICKS 20

typedef struct {
  uint32_t player1_next_allowed_shot_frame;
  uint32_t player2_next_allowed_shot_frame;
} CombatState;

void combat_init(CombatState *combat);
void combat_reset(CombatState *combat);

bool combat_try_shoot(CombatState *combat, int shooter_num, const Player *shooter, Player *target, const Arena *arena, uint32_t frame_counter);

#endif
