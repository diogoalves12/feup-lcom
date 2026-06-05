/**
 * @file arena.h
 * @brief Arena grid, spawns, teleporters and breakable walls.
 *
 * Tile coordinates use row and column. 
 * Pixel coordinates are derived from grid coordinates using TILE_SIZE. 
 * arena_init() applies the selected layout and stores the spawn and teleporter positions.
 */
#ifndef ARENA_H
#define ARENA_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"
#include "types.h"

#define ARENA_PIXEL_WIDTH (ARENA_COLS * TILE_SIZE)
#define DEFAULT_ARENA_DIFFICULTY ARENA_MEDIUM

/**
 * @brief  Type of a tile in the arena grid.
 */
typedef enum {
  TILE_FLOOR = 0,      /**< Passable floor. */
  TILE_WALL,           /**< Indestructible wall. */
  TILE_BREAKABLE_WALL, /**< Wall that can be destroyed by bullets. */
  TILE_TELEPORTER_A,   /**< First teleporter pad. */
  TILE_TELEPORTER_B,   /**< Second teleporter pad. */
  TILE_PLAYER1_SPAWN,  /**< Player 1 spawn location (treated as floor). */
  TILE_PLAYER2_SPAWN   /**< Player 2 spawn location (treated as floor). */
} TileType;

/**
 * @brief Selects the arena layout and item density for a match.
 */
typedef enum {
  ARENA_EASY = 0,
  ARENA_MEDIUM,   
  ARENA_HARD      
} ArenaDifficulty;

/**
 * @brief Tile in the arena grid.
 */
typedef struct {
  TileType type; 
  uint8_t  hp;   /**< Remaining hit points, only used for TILE_BREAKABLE_WALL */
} ArenaTile;

/**
 * @brief Arena state for one match.
 */
typedef struct {
  ArenaDifficulty difficulty;            
  ArenaTile tiles[ARENA_ROWS][ARENA_COLS]; 
  Position player1_spawn;          
  Position player2_spawn;          
  Position teleporter_a;           
  Position teleporter_b;           
  bool has_teleporter_a;       /**< True if teleporter A exists. */
  bool has_teleporter_b;       /**< True if teleporter B exists. */
} Arena;

/**
 * @brief Initializes the arena with the given difficulty.
 *
 * Fills the grid, applies the selected layout and stores the special tile positions used during gameplay.
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
 * @brief Places a teleporter tile and updates the teleporter position.
 */
void arena_set_teleporter(Arena *arena, int row, int col, TileType type);

/**
 * @brief Applies damage to a tile, destroying it if HP reaches zero.
 *
 * Only affects TILE_BREAKABLE_WALL tiles. 
 * Returns true if the tile was destroyed, which results in HP reaching zero and tile changed to TILE_FLOOR.
 */
bool arena_damage_tile(Arena *arena, int row, int col, uint8_t damage);

/**
 * @brief Returns the spawn position for Player 1.
 */
Position arena_get_player1_spawn(const Arena *arena);

/**
 * @brief Returns the spawn position for Player 2.
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
 * Given a teleporter tile type, writes the pixel space center of the paired teleporter into @p destination.
 */
bool arena_get_teleporter_destination(const Arena *arena, TileType from, Position *destination);

#endif
