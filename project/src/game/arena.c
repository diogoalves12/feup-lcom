#include "arena.h"

#include <stddef.h>

#include "arena_draw.h"
#include "arena_layout.h"

static const int player1_spawn_row = ARENA_ROWS / 2;
static const int player1_spawn_col = 3;
static const int player2_spawn_row = ARENA_ROWS / 2;
static const int player2_spawn_col = ARENA_COLS - 4;

static bool inside_bounds(int row, int col) {
  return row >= 0 && row < ARENA_ROWS && col >= 0 && col < ARENA_COLS;
}

static Position tile_center(int row, int col) {
  Position position;
  position.x = (col * TILE_SIZE) + (TILE_SIZE / 2);
  position.y = (row * TILE_SIZE) + (TILE_SIZE / 2);
  return position;
}

void arena_set_tile(Arena *arena, int row, int col, TileType type) {
  if (arena == NULL || !inside_bounds(row, col)) {
    return;
  }

  arena->tiles[row][col].type = type;
}

// Clears all cells to floor.
static void clear_tiles(Arena *arena) {
  for (int row = 0; row < ARENA_ROWS; row++) {
    for (int col = 0; col < ARENA_COLS; col++) {
      arena_set_tile(arena, row, col, TILE_FLOOR);
    }
  }
}

// Adds the outer wall border.
static void add_borders(Arena *arena) {
  for (int col = 0; col < ARENA_COLS; col++) {
    arena_set_tile(arena, 0, col, TILE_WALL);
    arena_set_tile(arena, ARENA_ROWS - 1, col, TILE_WALL);
  }

  for (int row = 0; row < ARENA_ROWS; row++) {
    arena_set_tile(arena, row, 0, TILE_WALL);
    arena_set_tile(arena, row, ARENA_COLS - 1, TILE_WALL);
  }
}

static void place_spawns(Arena *arena) {
  arena->player1_spawn = tile_center(player1_spawn_row, player1_spawn_col);
  arena->player2_spawn = tile_center(player2_spawn_row, player2_spawn_col);

  arena_set_tile(arena, player1_spawn_row, player1_spawn_col, TILE_PLAYER1_SPAWN);
  arena_set_tile(arena, player2_spawn_row, player2_spawn_col, TILE_PLAYER2_SPAWN);
}

int arena_init(Arena *arena, ArenaDifficulty difficulty) {
  if (arena == NULL) {
    return 1;
  }

  arena->difficulty = difficulty;
  clear_tiles(arena);
  add_borders(arena);
  arena_layout_apply(arena, difficulty);
  place_spawns(arena);

  return 0;
}

int arena_draw(const Arena *arena) {
  return arena_draw_tiles(arena);
}

Position arena_get_player1_spawn(const Arena *arena) {
  if (arena == NULL) {
    return (Position) {0, 0};
  }

  return arena->player1_spawn;
}

Position arena_get_player2_spawn(const Arena *arena) {
  if (arena == NULL) {
    return (Position) {0, 0};
  }

  return arena->player2_spawn;
}

TileType arena_get_tile_type(const Arena *arena, int row, int col) {
  if (arena == NULL || !inside_bounds(row, col)) {
    return TILE_WALL;
  }

  return arena->tiles[row][col].type;
}

bool arena_is_wall_tile(TileType type) {
  return type == TILE_WALL;
}
