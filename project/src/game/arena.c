#include "arena.h"

#include <stddef.h>

#include "renderer.h"

#define ARENA_WALL_HP 255
#define ARENA_DESTRUCTIBLE_HP 3

#define ARENA_WALL_COLOR 0x6A6A6A
#define ARENA_DESTRUCTIBLE_COLOR 0xA35A1F
#define ARENA_ITEM_COLOR 0x2DBE7F
#define ARENA_PLAYER1_SPAWN_COLOR 0x3A7BFF
#define ARENA_PLAYER2_SPAWN_COLOR 0xD96A2A

// Difficulty now controls how much extra tactical cover is layered onto the fixed base layout.
typedef struct {
  uint8_t random_cover_pairs;
  uint8_t destructible_groups;
  uint8_t contested_items;
} ArenaGenerationConfig;

typedef struct {
  int row;
  int col;
  int width;
  int height;
} ArenaRect;

static const int player1_spawn_row = ARENA_ROWS / 2;
static const int player1_spawn_col = 3;
static const int player2_spawn_row = ARENA_ROWS / 2;
static const int player2_spawn_col = ARENA_COLS - 4;

// Small deterministic pseudo-random generator so arena layouts can be reproduced from a seed.
static uint32_t arena_next_random(uint32_t *state) {
  *state = (*state * 1664525u) + 1013904223u;
  return *state;
}

static int arena_random_between(uint32_t *state, int min_value, int max_value) {
  uint32_t range = (uint32_t) (max_value - min_value + 1);
  return min_value + (int) (arena_next_random(state) % range);
}

// Maps difficulty to extra cover density without changing the readable base flow of the map.
static ArenaGenerationConfig arena_get_generation_config(ArenaDifficulty difficulty) {
  switch (difficulty) {
    case ARENA_EASY:
      return (ArenaGenerationConfig) {1, 2, 1};
    case ARENA_HARD:
      return (ArenaGenerationConfig) {4, 5, 3};
    case ARENA_MEDIUM:
    default:
      return (ArenaGenerationConfig) {2, 3, 2};
  }
}

static bool arena_is_inside_bounds(int row, int col) {
  return row >= 0 && row < ARENA_ROWS && col >= 0 && col < ARENA_COLS;
}

// Keeps both spawn regions clear so future players can move immediately after spawning.
static bool arena_is_spawn_zone(int row, int col) {
  bool player1_zone = row >= player1_spawn_row - 2 && row <= player1_spawn_row + 2 &&
                      col >= player1_spawn_col - 2 && col <= player1_spawn_col + 3;
  bool player2_zone = row >= player2_spawn_row - 2 && row <= player2_spawn_row + 2 &&
                      col >= player2_spawn_col - 3 && col <= player2_spawn_col + 2;

  return player1_zone || player2_zone;
}

static void arena_set_tile(Arena *arena, int row, int col, TileType type, uint8_t hp) {
  if (!arena_is_inside_bounds(row, col)) {
    return;
  }

  arena->tiles[row][col].type = type;
  arena->tiles[row][col].hp = hp;
}

static void arena_clear_tiles(Arena *arena) {
  for (int row = 0; row < ARENA_ROWS; row++) {
    for (int col = 0; col < ARENA_COLS; col++) {
      arena_set_tile(arena, row, col, TILE_EMPTY, 0);
    }
  }
}

static void arena_add_border_walls(Arena *arena) {
  for (int col = 0; col < ARENA_COLS; col++) {
    arena_set_tile(arena, 0, col, TILE_WALL, ARENA_WALL_HP);
    arena_set_tile(arena, ARENA_ROWS - 1, col, TILE_WALL, ARENA_WALL_HP);
  }

  for (int row = 0; row < ARENA_ROWS; row++) {
    arena_set_tile(arena, row, 0, TILE_WALL, ARENA_WALL_HP);
    arena_set_tile(arena, row, ARENA_COLS - 1, TILE_WALL, ARENA_WALL_HP);
  }
}

static Position arena_tile_center_position(int row, int col) {
  Position position;
  position.x = (col * TILE_SIZE) + (TILE_SIZE / 2);
  position.y = (row * TILE_SIZE) + (TILE_SIZE / 2);
  return position;
}

static void arena_place_spawn_tiles(Arena *arena) {
  arena->player1_spawn = arena_tile_center_position(player1_spawn_row, player1_spawn_col);
  arena->player2_spawn = arena_tile_center_position(player2_spawn_row, player2_spawn_col);

  arena_set_tile(arena, player1_spawn_row, player1_spawn_col, TILE_PLAYER1_SPAWN, 0);
  arena_set_tile(arena, player2_spawn_row, player2_spawn_col, TILE_PLAYER2_SPAWN, 0);
}

// Prevents generated structures from replacing border walls or protected spawn space.
static bool arena_can_fill_tile(const Arena *arena, int row, int col) {
  if (!arena_is_inside_bounds(row, col)) {
    return false;
  }

  if (row == 0 || row == ARENA_ROWS - 1 || col == 0 || col == ARENA_COLS - 1) {
    return false;
  }

  if (arena_is_spawn_zone(row, col)) {
    return false;
  }

  return arena->tiles[row][col].type == TILE_EMPTY;
}

static bool arena_can_place_rect(const Arena *arena, ArenaRect rect) {
  for (int row = rect.row; row < rect.row + rect.height; row++) {
    for (int col = rect.col; col < rect.col + rect.width; col++) {
      if (!arena_can_fill_tile(arena, row, col)) {
        return false;
      }
    }
  }

  return true;
}

static void arena_add_rect(Arena *arena, ArenaRect rect, TileType type, uint8_t hp) {
  if (!arena_can_place_rect(arena, rect)) {
    return;
  }

  for (int row = rect.row; row < rect.row + rect.height; row++) {
    for (int col = rect.col; col < rect.col + rect.width; col++) {
      arena_set_tile(arena, row, col, type, hp);
    }
  }
}

// Adds a left structure and a mirrored right structure as full rectangles instead of scattered tiles.
static void arena_add_symmetric_rect(Arena *arena, ArenaRect left_rect, TileType type, uint8_t hp) {
  ArenaRect right_rect;
  right_rect.row = left_rect.row;
  right_rect.width = left_rect.width;
  right_rect.height = left_rect.height;
  right_rect.col = ARENA_COLS - left_rect.col - left_rect.width;

  arena_add_rect(arena, left_rect, type, hp);
  arena_add_rect(arena, right_rect, type, hp);
}

// The base layout creates readable lanes and central line-of-sight breaks before any variations are added.
static void arena_add_main_cover(Arena *arena) {
  arena_add_symmetric_rect(arena, (ArenaRect) {4, 8, 3, 2}, TILE_WALL, ARENA_WALL_HP);
  arena_add_symmetric_rect(arena, (ArenaRect) {18, 8, 3, 2}, TILE_WALL, ARENA_WALL_HP);
  arena_add_symmetric_rect(arena, (ArenaRect) {8, 11, 2, 3}, TILE_WALL, ARENA_WALL_HP);
  arena_add_symmetric_rect(arena, (ArenaRect) {13, 11, 2, 3}, TILE_WALL, ARENA_WALL_HP);

  arena_add_rect(arena, (ArenaRect) {6, 14, 2, 4}, TILE_WALL, ARENA_WALL_HP);
  arena_add_rect(arena, (ArenaRect) {14, 14, 2, 4}, TILE_WALL, ARENA_WALL_HP);
}

// Side cover gives each player early protection without trapping the spawn exits.
static void arena_add_side_cover(Arena *arena) {
  arena_add_symmetric_rect(arena, (ArenaRect) {7, 5, 2, 2}, TILE_WALL, ARENA_WALL_HP);
  arena_add_symmetric_rect(arena, (ArenaRect) {15, 5, 2, 2}, TILE_WALL, ARENA_WALL_HP);
  arena_add_symmetric_rect(arena, (ArenaRect) {10, 9, 2, 1}, TILE_WALL, ARENA_WALL_HP);
  arena_add_symmetric_rect(arena, (ArenaRect) {13, 9, 2, 1}, TILE_WALL, ARENA_WALL_HP);
}

// Destructible cover sits in contested lanes so later gameplay can open new routes over time.
static void arena_add_destructible_cover(Arena *arena) {
  arena_add_symmetric_rect(arena, (ArenaRect) {6, 12, 1, 2}, TILE_DESTRUCTIBLE_WALL, ARENA_DESTRUCTIBLE_HP);
  arena_add_symmetric_rect(arena, (ArenaRect) {16, 12, 1, 2}, TILE_DESTRUCTIBLE_WALL, ARENA_DESTRUCTIBLE_HP);
  arena_add_rect(arena, (ArenaRect) {11, 13, 1, 1}, TILE_DESTRUCTIBLE_WALL, ARENA_DESTRUCTIBLE_HP);
  arena_add_rect(arena, (ArenaRect) {12, 18, 1, 1}, TILE_DESTRUCTIBLE_WALL, ARENA_DESTRUCTIBLE_HP);
}

// Items are placed in clear, readable lanes so they are visible and fairly contestable.
static void arena_add_item_spawns(Arena *arena, ArenaDifficulty difficulty) {
  arena_add_symmetric_rect(arena, (ArenaRect) {5, 3, 1, 1}, TILE_ITEM_SPAWN, 0);
  arena_add_symmetric_rect(arena, (ArenaRect) {18, 3, 1, 1}, TILE_ITEM_SPAWN, 0);

  if (difficulty >= ARENA_MEDIUM) {
    arena_add_rect(arena, (ArenaRect) {8, 16, 1, 1}, TILE_ITEM_SPAWN, 0);
    arena_add_rect(arena, (ArenaRect) {15, 13, 1, 1}, TILE_ITEM_SPAWN, 0);
  }

  if (difficulty == ARENA_HARD) {
    arena_add_symmetric_rect(arena, (ArenaRect) {11, 6, 1, 1}, TILE_ITEM_SPAWN, 0);
  }
}

static ArenaRect arena_random_left_cover_variant(uint32_t *seed) {
  switch (arena_random_between(seed, 0, 2)) {
    case 0:
      return (ArenaRect) {3, 12, 2, 3};
    case 1:
      return (ArenaRect) {9, 7, 3, 1};
    default:
      return (ArenaRect) {19, 11, 2, 2};
  }
}

// Adds small seeded variations so different runs feel fresh without turning into scattered clutter.
static void arena_add_random_variations(Arena *arena, ArenaDifficulty difficulty, uint32_t *seed) {
  ArenaGenerationConfig config = arena_get_generation_config(difficulty);

  for (uint8_t i = 0; i < config.random_cover_pairs; i++) {
    ArenaRect variation = arena_random_left_cover_variant(seed);
    arena_add_symmetric_rect(arena, variation, TILE_WALL, ARENA_WALL_HP);
  }

  for (uint8_t i = 0; i < config.destructible_groups; i++) {
    int row = arena_random_between(seed, 4, ARENA_ROWS - 5);
    int col = arena_random_between(seed, 11, 19);
    arena_add_rect(arena, (ArenaRect) {row, col, 1, 1}, TILE_DESTRUCTIBLE_WALL, ARENA_DESTRUCTIBLE_HP);
  }

  for (uint8_t i = 0; i < config.contested_items; i++) {
    int row = arena_random_between(seed, 6, ARENA_ROWS - 7);
    int col = arena_random_between(seed, 14, 17);
    arena_add_rect(arena, (ArenaRect) {row, col, 1, 1}, TILE_ITEM_SPAWN, 0);
  }
}

static uint32_t arena_marker_offset(void) {
  return TILE_SIZE / 4;
}

static uint32_t arena_marker_size(void) {
  return TILE_SIZE / 2;
}

// Spawn and item tiles are drawn as smaller markers inside their cell instead of full blocks.
static int arena_draw_marker_tile(int row, int col, uint32_t color) {
  uint16_t offset = (uint16_t) arena_marker_offset();
  uint16_t size = (uint16_t) arena_marker_size();
  uint16_t x = (uint16_t) (col * TILE_SIZE + offset);
  uint16_t y = (uint16_t) (row * TILE_SIZE + offset);

  return renderer_draw_rectangle(x, y, size, size, color);
}

// Generation pipeline: clear tiles, add borders, place spawns, build the base cover, then layer controlled variations.
int arena_init(Arena *arena, ArenaDifficulty difficulty, uint32_t seed) {
  if (arena == NULL) {
    return 1;
  }

  arena->difficulty = difficulty;

  arena_clear_tiles(arena);
  arena_add_border_walls(arena);
  arena_place_spawn_tiles(arena);
  arena_add_main_cover(arena);
  arena_add_side_cover(arena);
  arena_add_destructible_cover(arena);
  arena_add_item_spawns(arena, difficulty);
  arena_add_random_variations(arena, difficulty, &seed);

  return 0;
}

// Draws the tile grid with simple rectangles so the arena is visible before sprites exist.
int arena_draw(const Arena *arena) {
  if (arena == NULL) {
    return 1;
  }

  for (int row = 0; row < ARENA_ROWS; row++) {
    for (int col = 0; col < ARENA_COLS; col++) {
      uint16_t x = (uint16_t) (col * TILE_SIZE);
      uint16_t y = (uint16_t) (row * TILE_SIZE);
      TileType type = arena->tiles[row][col].type;

      switch (type) {
        case TILE_WALL:
          if (renderer_draw_rectangle(x, y, TILE_SIZE, TILE_SIZE, ARENA_WALL_COLOR) != 0) {
            return 1;
          }
          break;
        case TILE_DESTRUCTIBLE_WALL:
          if (renderer_draw_rectangle(x, y, TILE_SIZE, TILE_SIZE, ARENA_DESTRUCTIBLE_COLOR) != 0) {
            return 1;
          }
          break;
        case TILE_ITEM_SPAWN:
          if (arena_draw_marker_tile(row, col, ARENA_ITEM_COLOR) != 0) {
            return 1;
          }
          break;
        case TILE_PLAYER1_SPAWN:
          if (arena_draw_marker_tile(row, col, ARENA_PLAYER1_SPAWN_COLOR) != 0) {
            return 1;
          }
          break;
        case TILE_PLAYER2_SPAWN:
          if (arena_draw_marker_tile(row, col, ARENA_PLAYER2_SPAWN_COLOR) != 0) {
            return 1;
          }
          break;
        case TILE_EMPTY:
        default:
          break;
      }
    }
  }

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
  if (arena == NULL || !arena_is_inside_bounds(row, col)) {
    return TILE_WALL;
  }

  return arena->tiles[row][col].type;
}

bool arena_is_solid_tile(TileType type) {
  return type == TILE_WALL || type == TILE_DESTRUCTIBLE_WALL;
}

bool arena_is_destructible_tile(TileType type) {
  return type == TILE_DESTRUCTIBLE_WALL;
}
