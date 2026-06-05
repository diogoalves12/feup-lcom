/**
 * @file item_draw.h
 * @brief Item sprite rendering.
 *
 * Health pickups use a four frame animation driven by the frame counter.
 */
#ifndef ITEM_DRAW_H
#define ITEM_DRAW_H

#include <stdint.h>

#include "item.h"

/**
 * @brief Loads item sprites.
 */
int item_view_load_assets(void);

/**
 * @brief Frees all item sprites.
 */
void item_view_destroy_assets(void);

/**
 * @brief Draws all active items in the item pool.
 *
 * Inactive items are skipped.
 */
int item_view_draw(const ItemPool *pool, uint32_t frame_counter);

#endif
