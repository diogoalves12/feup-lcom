/**
 * @file collision.h
 * @brief Player and wall collision checks.
 *
 * Collision is tested against the player's rectangular hitbox at a candidate
 * position. No game state is changed.
 */
#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>

#include "arena.h"
#include "player.h"

/**
 * @brief Tests whether a player would collide with a wall at the given position.
 *
 * Uses the player's width and height as the bounding box and checks every
 * overlapping arena tile. Does not move the player.
 */
bool collision_player_walls(const Arena *arena, const Player *player, Position position);

#endif
