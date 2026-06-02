#ifndef PROJECT_GAME_COMBAT_H
#define PROJECT_GAME_COMBAT_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "player.h"

// Damage applied per successful hit.
#define COMBAT_DAMAGE 1

// Hitscan setting: Shots are resolved instantly, no bullet object is stored or updated.
#define COMBAT_RAY_STEP 4
#define COMBAT_AIM_TOLERANCE 12
#define COMBAT_MAX_DISTANCE 900

// Delay between shots, measured in timer frames.
#define COMBAT_SHOT_COOLDOWN_TICKS 20

typedef struct {
  uint32_t player1_next_allowed_shot_frame;
  uint32_t player2_next_allowed_shot_frame;
} CombatState;

void combat_init(CombatState *combat);
void combat_reset(CombatState *combat);

// Attempts one shot from shooter to target.
// On hit, applies damage and returns true.
bool combat_try_shoot(CombatState *combat, int shooter_num, const Player *shooter, Player *target, const Arena *arena, uint32_t frame_counter);

#endif
