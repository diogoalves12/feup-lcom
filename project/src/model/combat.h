/**
 * @file combat.h
 * @brief Shooting logic and bullet feedback.
 *
 * A shot is resolved by stepping a ray from the shooter in the facing direction. 
 * The ray stops on a wall, breakable wall, target hitbox. 
 * The last shot is stored so the view can draw feedback.
 */
#ifndef COMBAT_H
#define COMBAT_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "config.h"
#include "player.h"

/**
 * @brief Visual record of the most recent shot.
 */
typedef struct {
  bool active;           /**< True while the shot effect should be rendered. */
  Position start;        /**< Ray origin in pixel space. */
  Position end;          /**< Ray end (hit point or max-range position). */
  float angle;           /**< Facing angle at the time of the shot (radians). */
  int shooter_id;        /**< 1 for Player 1, 2 for Player 2. */
  bool hit;              /**< True if the ray hit the target player. */
  uint32_t start_frame;  /**< Frame counter when the shot was fired. */
  uint32_t expire_frame; /**< Frame counter when the effect should be cleared. */
} ShotEffect;

/**
 * @brief Per match combat state.
 */
typedef struct {
  ShotEffect last_shot; /**< Most recent shot effect for rendering. */
} CombatState;

/**
 * @brief Clears the active shot effect.
 */
void combat_init(CombatState *combat);

/**
 * @brief Attempts to fire a shot from @p shooter toward @p target.
 *
 * The shooter cooldown is checked and updated here. A hit damages the target.
 * A breakable wall hit damages the wall and stops the ray.
 */
bool combat_try_shoot(CombatState *combat, int shooter_id, Player *shooter, Player *target, Arena *arena, uint32_t frame_counter);

#endif
