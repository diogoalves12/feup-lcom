#ifndef PROJECT_MODEL_ARENA_H
#define PROJECT_MODEL_ARENA_H

#include <stdbool.h>
#include <stdint.h>

#include "types.h"

#define TILE_SIZE 20
#define ARENA_COLS 40
#define ARENA_ROWS 30
#define ARENA_PIXEL_WIDTH (ARENA_COLS * TILE_SIZE)

#define BREAKABLE_WALL_HP 3

typedef enum {
  TILE_FLOOR = 0,
  TILE_WALL,
  TILE_BREAKABLE_WALL,
  TILE_TELEPORTER_A,
  TILE_TELEPORTER_B,
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
  uint8_t  hp;
} ArenaTile;

typedef struct {
  ArenaDifficulty difficulty;
  ArenaTile tiles[ARENA_ROWS][ARENA_COLS];
  Position player1_spawn;
  Position player2_spawn;
  Position teleporter_a;
  Position teleporter_b;
  bool     has_teleporter_a;
  bool     has_teleporter_b;
} Arena;

int arena_init(Arena *arena, ArenaDifficulty difficulty);

void arena_set_tile(Arena *arena, int row, int col, TileType type);
void arena_set_breakable_wall(Arena *arena, int row, int col);
void arena_set_teleporter(Arena *arena, int row, int col, TileType type);
bool arena_damage_tile(Arena *arena, int row, int col, uint8_t damage);

Position arena_get_player1_spawn(const Arena *arena);
Position arena_get_player2_spawn(const Arena *arena);

TileType arena_get_tile_type(const Arena *arena, int row, int col);

bool arena_is_wall_tile(TileType type);
bool arena_is_breakable_wall_tile(TileType type);
bool arena_is_teleporter_tile(TileType type);

bool arena_get_teleporter_destination(const Arena *arena, TileType from, Position *destination);

#endif
