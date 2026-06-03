#ifndef PROJECT_MODEL_COLLISION_H
#define PROJECT_MODEL_COLLISION_H

#include <stdbool.h>

#include "arena.h"
#include "player.h"

bool collision_player_walls(const Arena *arena, const Player *player, Position position);

#endif
