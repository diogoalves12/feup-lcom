/**
 * @file item.h
 * @brief Health pickups and item manager state.
 *
 * Items are placed at match start. Collected items are hidden and become
 * active again after a fixed respawn delay.
 */
#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "player.h"
#include "types.h"

#define MAX_ITEMS                 8   /**< Maximum number of simultaneous items. */
#define HEALTH_PICKUP_HEAL_AMOUNT 1   /**< Hit points restored when a health pickup is collected. */
#define HEALTH_PICKUP_RESPAWN_FRAMES 600 /**< Frames before a collected item reappears. */
#define HEALTH_PICKUP_FRAME_COUNT 4   /**< Number of animation frames for the health pickup sprite. */

/**
 * @brief Pickup type.
 */
typedef enum {
  ITEM_HEALTH = 0 /**< Restores HEALTH_PICKUP_HEAL_AMOUNT hit points on collection. */
} ItemType;

/**
 * @brief One pickup in the arena.
 */
typedef struct {
  ItemType type;          /**< Kind of pickup. */
  Position position;      /**< Pixel-space center position on the arena floor. */
  bool     active;        /**< True when the item is visible and can be collected. */
  uint32_t respawn_frame; /**< Frame counter value at which the item becomes active again. */
} Item;

/**
 * @brief Fixed item pool for one match.
 */
typedef struct {
  Item items[MAX_ITEMS]; /**< Fixed-size item pool. */
  int  count;            /**< Number of items placed for the current difficulty. */
} ItemManager;

/**
 * @brief Initializes the item manager and places items according to difficulty.
 */
void item_manager_init(ItemManager *manager, ArenaDifficulty difficulty);

/**
 * @brief Checks collection and respawn timing.
 *
 * Called once per gameplay frame. Health pickups heal the first player
 * that overlaps them.
 */
void item_manager_update(ItemManager *manager, Player *player1, Player *player2, uint32_t frame_counter);

#endif
