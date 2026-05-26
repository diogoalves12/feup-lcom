#ifndef PROJECT_GAME_ARENA_H
#define PROJECT_GAME_ARENA_H

#include <stdbool.h>
#include <stdint.h>

// Tile dimensions used to map the arena grid to the 800x600 screen.
#define TILE_SIZE 25
// Number of tile columns that fit horizontally on screen.
#define ARENA_COLS 32
// Number of tile rows that fit vertically on screen.
#define ARENA_ROWS 24

// Integer pixel position used for spawn points and gameplay entities.
typedef struct {
  int x;
  int y;
} Position;

// Logical tile categories used to generate, draw, and query the arena.
typedef enum {
  TILE_EMPTY = 0,
  TILE_WALL,
  TILE_DESTRUCTIBLE_WALL,
  TILE_ITEM_SPAWN,
  TILE_PLAYER1_SPAWN,
  TILE_PLAYER2_SPAWN
} TileType;

// Difficulty controls how dense the generated arena becomes.
typedef enum {
  ARENA_EASY = 0,
  ARENA_MEDIUM,
  ARENA_HARD
} ArenaDifficulty;

#define DEFAULT_ARENA_DIFFICULTY ARENA_MEDIUM

// One arena cell, including tile kind and future wall durability.
typedef struct {
  TileType type;
  uint8_t hp;
} ArenaTile;

// Match arena state, including the generated grid and both player spawns.
typedef struct {
  ArenaDifficulty difficulty;
  ArenaTile tiles[ARENA_ROWS][ARENA_COLS];
  Position player1_spawn;
  Position player2_spawn;
} Arena;

/**
 * Initializes a new arena layout for the selected difficulty and seed.
 */
int arena_init(Arena *arena, ArenaDifficulty difficulty, uint32_t seed);

/**
 * Draws the full arena grid using the renderer rectangle API.
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
 * Returns the tile type at a grid position for future gameplay queries.
 */
TileType arena_get_tile_type(const Arena *arena, int row, int col);

/**
 * Tells whether a tile blocks movement or shots.
 */
bool arena_is_solid_tile(TileType type);

/**
 * Tells whether a tile can be damaged and destroyed.
 */
bool arena_is_destructible_tile(TileType type);

#endif
