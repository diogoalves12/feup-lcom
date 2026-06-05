/**
 * @file arena.h
 * @brief Arena grid, spawns, teleporters and breakable walls.
 *
 * Tile coordinates use row and column. Pixel coordinates are derived from
 * those grid coordinates using TILE_SIZE. arena_init() applies the selected
 * layout and caches spawn and teleporter positions for fast access.
 */
#ifndef ARENA_H
#define ARENA_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"
#include "types.h"

#define ARENA_PIXEL_WIDTH (ARENA_COLS * TILE_SIZE) /**< Arena width in pixels. */

/**
 * @brief Classifies each cell in the arena grid.
 */
typedef enum {
  TILE_FLOOR = 0,      /**< Passable floor. */
  TILE_WALL,           /**< Solid indestructible wall. */
  TILE_BREAKABLE_WALL, /**< Wall that can be destroyed by bullets. */
  TILE_TELEPORTER_A,   /**< First teleporter pad. */
  TILE_TELEPORTER_B,   /**< Second teleporter pad. */
  TILE_PLAYER1_SPAWN,  /**< Player 1 spawn location (treated as floor at runtime). */
  TILE_PLAYER2_SPAWN   /**< Player 2 spawn location (treated as floor at runtime). */
} TileType;

/**
 * @brief Selects the wall layout and item density for a match.
 */
typedef enum {
  ARENA_EASY = 0, /**< Open layout with few obstacles. */
  ARENA_MEDIUM,   /**< Balanced layout with moderate cover. */
  ARENA_HARD      /**< Dense layout with many walls and breakable blocks. */
} ArenaDifficulty;

/** @brief Default difficulty used when the player has not made a selection. */
#define DEFAULT_ARENA_DIFFICULTY ARENA_MEDIUM

/**
 * @brief A single cell in the arena grid.
 */
typedef struct {
  TileType type; /**< Tile classification. */
  uint8_t  hp;   /**< Remaining hit points (only meaningful for TILE_BREAKABLE_WALL). */
} ArenaTile;

/**
 * @brief Arena state for one match.
 */
typedef struct {
  ArenaDifficulty difficulty;             /**< Difficulty used to initialize this arena. */
  ArenaTile       tiles[ARENA_ROWS][ARENA_COLS]; /**< 2D tile grid. */
  Position        player1_spawn;          /**< Cached pixel spawn position for Player 1. */
  Position        player2_spawn;          /**< Cached pixel spawn position for Player 2. */
  Position        teleporter_a;           /**< Cached pixel center of teleporter A. */
  Position        teleporter_b;           /**< Cached pixel center of teleporter B. */
  bool            has_teleporter_a;       /**< True if teleporter A exists in this layout. */
  bool            has_teleporter_b;       /**< True if teleporter B exists in this layout. */
} Arena;

/**
 * @brief Initializes the arena with the given difficulty.
 *
 * Fills the grid, applies the selected layout and stores the special tile
 * positions used during gameplay.
 */
int arena_init(Arena *arena, ArenaDifficulty difficulty);

/**
 * @brief Sets a tile to the given type, resetting its HP.
 */
void arena_set_tile(Arena *arena, int row, int col, TileType type);

/**
 * @brief Places a breakable wall tile with full HP at (row, col).
 */
void arena_set_breakable_wall(Arena *arena, int row, int col);

/**
 * @brief Places a teleporter tile and updates the cached teleporter position.
 */
void arena_set_teleporter(Arena *arena, int row, int col, TileType type);

/**
 * @brief Applies damage to a tile, destroying it if HP reaches zero.
 *
 * Only affects TILE_BREAKABLE_WALL tiles. Returns true if the tile was
 * destroyed (HP reached zero and tile changed to TILE_FLOOR).
 */
bool arena_damage_tile(Arena *arena, int row, int col, uint8_t damage);

/**
 * @brief Returns the cached spawn position for Player 1.
 */
Position arena_get_player1_spawn(const Arena *arena);

/**
 * @brief Returns the cached spawn position for Player 2.
 */
Position arena_get_player2_spawn(const Arena *arena);

/**
 * @brief Returns the type of the tile at (row, col).
 *
 * Returns TILE_FLOOR for out-of-bounds coordinates.
 */
TileType arena_get_tile_type(const Arena *arena, int row, int col);

/**
 * @brief Returns true if the tile type blocks player movement.
 */
bool arena_is_wall_tile(TileType type);

/**
 * @brief Returns true if the tile type is a breakable wall.
 */
bool arena_is_breakable_wall_tile(TileType type);

/**
 * @brief Returns true if the tile type is a teleporter pad.
 */
bool arena_is_teleporter_tile(TileType type);

/**
 * @brief Resolves the destination position of a teleporter.
 *
 * Given a teleporter tile type, writes the pixel-space center of the
 * paired teleporter into @p destination.
 */
bool arena_get_teleporter_destination(const Arena *arena, TileType from, Position *destination);

#endif
