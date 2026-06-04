#include "arena.h"

#include <stddef.h>

#include "arena_layout.h"

static const int PLAYER1_SPAWN_ROW = ARENA_ROWS / 2;
static const int PLAYER1_SPAWN_COL = 3;
static const int PLAYER2_SPAWN_ROW = ARENA_ROWS / 2;
static const int PLAYER2_SPAWN_COL = ARENA_COLS - 4;

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
  arena->tiles[row][col].hp = (type == TILE_BREAKABLE_WALL) ? BREAKABLE_WALL_HP : 0;
}

void arena_set_breakable_wall(Arena *arena, int row, int col) {
  arena_set_tile(arena, row, col, TILE_BREAKABLE_WALL);
}

void arena_set_teleporter(Arena *arena, int row, int col, TileType type) {
  if (arena == NULL || !inside_bounds(row, col)) return;
  if (type != TILE_TELEPORTER_A && type != TILE_TELEPORTER_B) return;

  arena_set_tile(arena, row, col, type);

  Position center = tile_center(row, col);
  if (type == TILE_TELEPORTER_A) {
    arena->teleporter_a = center;
    arena->has_teleporter_a = true;
  } else {
    arena->teleporter_b = center;
    arena->has_teleporter_b = true;
  }
}

bool arena_damage_tile(Arena *arena, int row, int col, uint8_t damage) {
  if (arena == NULL || !inside_bounds(row, col)) {
    return false;
  }

  ArenaTile *tile = &arena->tiles[row][col];
  if (tile->type != TILE_BREAKABLE_WALL) {
    return false;
  }

  if (damage >= tile->hp) {
    tile->type = TILE_FLOOR;
    tile->hp = 0;
    return true;
  }

  tile->hp -= damage;
  return false;
}

static void clear_tiles(Arena *arena) {
  for (int row = 0; row < ARENA_ROWS; row++) {
    for (int col = 0; col < ARENA_COLS; col++) {
      arena_set_tile(arena, row, col, TILE_FLOOR);
    }
  }
}

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
  arena->player1_spawn = tile_center(PLAYER1_SPAWN_ROW, PLAYER1_SPAWN_COL);
  arena->player2_spawn = tile_center(PLAYER2_SPAWN_ROW, PLAYER2_SPAWN_COL);

  arena_set_tile(arena, PLAYER1_SPAWN_ROW, PLAYER1_SPAWN_COL, TILE_PLAYER1_SPAWN);
  arena_set_tile(arena, PLAYER2_SPAWN_ROW, PLAYER2_SPAWN_COL, TILE_PLAYER2_SPAWN);
}

int arena_init(Arena *arena, ArenaDifficulty difficulty) {
  if (arena == NULL) {
    return 1;
  }

  arena->difficulty = difficulty;
  arena->has_teleporter_a = false;
  arena->has_teleporter_b = false;
  clear_tiles(arena);
  add_borders(arena);
  arena_layout_apply(arena, difficulty);
  place_spawns(arena);

  return 0;
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
  return type == TILE_WALL || type == TILE_BREAKABLE_WALL;
}

bool arena_is_breakable_wall_tile(TileType type) {
  return type == TILE_BREAKABLE_WALL;
}

bool arena_is_teleporter_tile(TileType type) {
  return type == TILE_TELEPORTER_A || type == TILE_TELEPORTER_B;
}

bool arena_get_teleporter_destination(const Arena *arena, TileType from, Position *destination) {
  if (arena == NULL || destination == NULL) return false;
  if (!arena->has_teleporter_a || !arena->has_teleporter_b) return false;

  if (from == TILE_TELEPORTER_A) {
    *destination = arena->teleporter_b;
    return true;
  }
  if (from == TILE_TELEPORTER_B) {
    *destination = arena->teleporter_a;
    return true;
  }
  return false;
}
