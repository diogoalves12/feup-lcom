#include "arena_draw.h"

#include <stddef.h>
#include <stdint.h>

#include "renderer.h"

#define FLOOR_COLOR 0x202020
#define FLOOR_ALT_COLOR 0x252525
#define WALL_COLOR 0x6A6A6A
#define PLAYER1_SPAWN_COLOR 0x3A7BFF
#define PLAYER2_SPAWN_COLOR 0xD96A2A

static Position tile_origin(int row, int col) {
  Position position;
  position.x = col * TILE_SIZE;
  position.y = row * TILE_SIZE;
  return position;
}

static uint16_t marker_offset(void) {
  return TILE_SIZE / 4;
}

static uint16_t marker_size(void) {
  return TILE_SIZE / 2;
}

static int draw_floor(int row, int col) {
  const Position origin = tile_origin(row, col);
  uint32_t color = ((row + col) % 2 == 0) ? FLOOR_COLOR : FLOOR_ALT_COLOR;
  return renderer_draw_rectangle((uint16_t) origin.x, (uint16_t) origin.y, TILE_SIZE, TILE_SIZE, color);
}

static int draw_wall(int row, int col) {
  const Position origin = tile_origin(row, col);
  return renderer_draw_rectangle((uint16_t) origin.x, (uint16_t) origin.y, TILE_SIZE, TILE_SIZE, WALL_COLOR);
}

static int draw_marker(int row, int col, uint32_t color) {
  const Position origin = tile_origin(row, col);
  uint16_t x = (uint16_t) (origin.x + marker_offset());
  uint16_t y = (uint16_t) (origin.y + marker_offset());
  return renderer_draw_rectangle(x, y, marker_size(), marker_size(), color);
}

int arena_view_draw(const Arena *arena) {
  if (arena == NULL) {
    return 1;
  }

  for (int row = 0; row < ARENA_ROWS; row++) {
    for (int col = 0; col < ARENA_COLS; col++) {
      if (draw_floor(row, col) != 0) {
        return 1;
      }

      switch (arena->tiles[row][col].type) {
        case TILE_WALL:
          if (draw_wall(row, col) != 0) {
            return 1;
          }
          break;
        case TILE_PLAYER1_SPAWN:
          if (draw_marker(row, col, PLAYER1_SPAWN_COLOR) != 0) {
            return 1;
          }
          break;
        case TILE_PLAYER2_SPAWN:
          if (draw_marker(row, col, PLAYER2_SPAWN_COLOR) != 0) {
            return 1;
          }
          break;
        case TILE_FLOOR:
        default:
          break;
      }
    }
  }

  return 0;
}
