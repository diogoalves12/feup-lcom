#ifndef PROJECT_GAME_ARENA_H
#define PROJECT_GAME_ARENA_H

#include <stdbool.h>

#include "types.h"

#define TILE_SIZE 25
#define ARENA_COLS 32
#define ARENA_ROWS 24

typedef enum {
  TILE_FLOOR = 0,
  TILE_WALL,
  TILE_PLAYER1_SPAWN,
  TILE_PLAYER2_SPAWN
} TileType;

typedef enum {
  ARENA_EASY = 0,
  ARENA_MEDIUM,
  ARENA_HARD
} ArenaDifficulty;

#define DEFAULT_ARENA_DIFFICULTY ARENA_MEDIUM

typedef struct {
  TileType type;
} ArenaTile;

typedef struct {
  ArenaDifficulty difficulty;
  ArenaTile tiles[ARENA_ROWS][ARENA_COLS];
  Position player1_spawn;
  Position player2_spawn;
} Arena;

/**
 * Initializes the arena for the selected difficulty.
 */
int arena_init(Arena *arena, ArenaDifficulty difficulty);

void arena_set_tile(Arena *arena, int row, int col, TileType type);

/**
 * Draws the arena grid.
 */
int arena_draw(const Arena *arena);

/**
 * Returns the screen-space spawn position for player 1.
 */
Position arena_get_player1_spawn(const Arena *arena);

/**
 * Returns the screen-space spawn position for player 2.
 */
Position arena_get_player2_spawn(const Arena *arena);

/**
 * Returns the tile type at a grid position.
 */
TileType arena_get_tile_type(const Arena *arena, int row, int col);

/**
 * Returns true when the tile is a wall.
 */
bool arena_is_wall_tile(TileType type);

#endif
