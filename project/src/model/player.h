/**
 * @file player.h
 * @brief Player state and movement helpers.
 *
 * Player stores position, angle, health, hitbox size, color and cooldowns.
 * Movement helpers keep position math in one place.
 */
#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"
#include "types.h"

#define PLAYER_FULL_ROTATION   6.2831852f /**< Full rotation in radians (2*pi). */

/**
 * @brief Runtime state for one player.
 */
typedef struct {
  Position position; /**< Current pixel-space center position. */
  float    angle;    /**< Current facing angle in radians (0 = right). */
  int      health;   /**< Remaining hit points. */
  bool     alive;    /**< False once health reaches zero. */
  uint16_t width;    /**< Hitbox width in pixels. */
  uint16_t height;   /**< Hitbox height in pixels. */
  uint32_t color;    /**< Player accent color for HUD rendering. */
  uint32_t next_teleport_frame; /**< Earliest gameplay frame when teleporting is allowed. */
  uint32_t next_shot_frame;     /**< Earliest frame when shooting is allowed. */
} Player;

/**
 * @brief Initializes a player at the given spawn position.
 */
void player_init(Player *player, Position spawn, float angle, uint32_t color);

/**
 * @brief Returns true if the player is still alive.
 */
bool player_is_alive(const Player *player);

/**
 * @brief Marks the player as dead regardless of remaining health.
 */
void player_kill(Player *player);

/**
 * @brief Reduces the player's health by @p damage.
 *
 * The player is killed when health reaches zero.
 */
void player_damage(Player *player, int damage);

/**
 * @brief Restores up to @p amount hit points, clamped to PLAYER_DEFAULT_HEALTH.
 */
void player_heal(Player *player, int amount);

/**
 * @brief Teleports the player to an exact pixel position.
 */
void player_set_position(Player *player, Position position);

/**
 * @brief Advances the player's facing angle by @p delta_angle radians.
 *
 * Wraps the angle to [0, PLAYER_FULL_ROTATION).
 */
void player_rotate(Player *player, float delta_angle);

/**
 * @brief Computes the pixel position reached by moving @p distance pixels forward.
 *
 * Does not modify the player or check for collisions.
 */
Position player_get_forward_position(const Player *player, float distance);

#endif
