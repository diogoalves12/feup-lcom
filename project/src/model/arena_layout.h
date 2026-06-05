/**
 * @file arena_layout.h
 * @brief Fixed arena layouts for each difficulty.
 *
 * This module only places tiles. 
 * Arena setup and positions stay in arena.c.
 */
#ifndef ARENA_LAYOUT_H
#define ARENA_LAYOUT_H

#include "arena.h"

/**
 * @brief Applies the wall, spawn and teleporter layout.
 */
void arena_layout_apply(Arena *arena, ArenaDifficulty difficulty);

#endif
