#ifndef PROJECT_GAME_COLLISION_H
#define PROJECT_GAME_COLLISION_H

#include <stdbool.h>

#include "arena.h"
#include "player.h"

// Checks the player box against wall tiles.
bool collision_player_walls(const Arena *arena, const Player *player, Position position);

#endif
