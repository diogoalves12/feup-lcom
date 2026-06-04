#include "arena_draw.h"

#include <stddef.h>
#include <stdint.h>

#include "renderer.h"

#define FLOOR_COLOR      0x1E1E1E
#define FLOOR_ALT_COLOR  0x232323
#define WALL_BASE_COLOR  0x505050
#define WALL_HIGHLIGHT   0x808080
#define WALL_SHADOW      0x303030

static Position tile_origin(int row, int col) {
  Position position;
  position.x = col * TILE_SIZE;
  position.y = row * TILE_SIZE;
  return position;
}

static int draw_floor(int row, int col) {
  const Position o = tile_origin(row, col);
  uint32_t color = ((row + col) % 2 == 0) ? FLOOR_COLOR : FLOOR_ALT_COLOR;
  return renderer_draw_rectangle((uint16_t)o.x, (uint16_t)o.y,
                                 TILE_SIZE, TILE_SIZE, color);
}

static int draw_wall(int row, int col) {
  const Position o = tile_origin(row, col);
  uint16_t x = (uint16_t)o.x;
  uint16_t y = (uint16_t)o.y;

  if (renderer_draw_rectangle(x, y, TILE_SIZE, TILE_SIZE, WALL_BASE_COLOR) != 0) return 1;
  if (renderer_draw_hline(x, y, TILE_SIZE, WALL_HIGHLIGHT) != 0) return 1;
  if (renderer_draw_rectangle(x, y, 1, TILE_SIZE, WALL_HIGHLIGHT) != 0) return 1;
  if (renderer_draw_hline(x, (uint16_t)(y + TILE_SIZE - 1), TILE_SIZE, WALL_SHADOW) != 0) return 1;
  if (renderer_draw_rectangle((uint16_t)(x + TILE_SIZE - 1), y, 1, TILE_SIZE, WALL_SHADOW) != 0) return 1;
  return 0;
}

int arena_view_draw(const Arena *arena) {
  if (arena == NULL) return 1;

  for (int row = 0; row < ARENA_ROWS; row++) {
    for (int col = 0; col < ARENA_COLS; col++) {
      if (draw_floor(row, col) != 0) return 1;
      if (arena->tiles[row][col].type == TILE_WALL) {
        if (draw_wall(row, col) != 0) return 1;
      }
    }
  }

  return 0;
}
