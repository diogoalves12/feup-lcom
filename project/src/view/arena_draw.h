/**
 * @file arena_draw.h
 * @brief Arena tile rendering.
 *
 * Floor, wall and breakable-wall tiles are drawn with colors. Teleporters
 * use animated sprites driven by the frame counter.
 */
#ifndef ARENA_DRAW_H
#define ARENA_DRAW_H

#include <stdint.h>

#include "arena.h"

/**
 * @brief Loads teleporter sprites.
 */
int arena_view_load_assets(void);

/**
 * @brief Frees all arena view sprites.
 */
void arena_view_destroy_assets(void);

/**
 * @brief Draws the complete arena tile grid for the current frame.
 */
int arena_view_draw(const Arena *arena, uint32_t frame_counter);

#endif
