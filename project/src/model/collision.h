/**
 * @file collision.h
 * @brief Player and wall collision checks.
 *
 * Collision is tested with the player rectangular hitbox at a candidate position.
 */
#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>

#include "arena.h"
#include "player.h"

/**
 * @brief Tests whether a player collides with a wall at the given position.
 *
 * Uses the player width and height as the bounding box and checks every overlapping arena tile. 
 * Returns true if any of the tiles is a wall or breakable wall.
 */
bool collision_player_walls(const Arena *arena, const Player *player, Position position);

#endif
