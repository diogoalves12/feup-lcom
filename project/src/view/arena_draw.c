#include "arena_draw.h"

#include <stddef.h>
#include <stdint.h>

#include "renderer.h"
#include "sprite.h"

#include "xpm/teleporters/teleporter_0.xpm"
#include "xpm/teleporters/teleporter_1.xpm"
#include "xpm/teleporters/teleporter_2.xpm"
#include "xpm/teleporters/teleporter_3.xpm"

#define FLOOR_COLOR      0x1E1E1E
#define FLOOR_ALT_COLOR  0x232323
#define WALL_BASE_COLOR  0x505050
#define WALL_HIGHLIGHT   0x808080
#define WALL_SHADOW      0x303030

#define BREAKABLE_FULL_COLOR    0x8B5A2B
#define BREAKABLE_MID_COLOR     0xB07040
#define BREAKABLE_LOW_COLOR     0xCC5533
#define BREAKABLE_HIGHLIGHT     0xD2A678
#define BREAKABLE_SHADOW        0x5A3A1B

#define TELEPORTER_FRAME_COUNT 4

static Sprite teleporter_frames[TELEPORTER_FRAME_COUNT];

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

static uint32_t breakable_color_for_hp(uint8_t hp) {
  if (hp >= BREAKABLE_WALL_HP) return BREAKABLE_FULL_COLOR;
  if (hp == 1)                 return BREAKABLE_LOW_COLOR;
  return BREAKABLE_MID_COLOR;
}

static int draw_breakable_wall(int row, int col, uint8_t hp) {
  const Position o = tile_origin(row, col);
  uint16_t x = (uint16_t)o.x;
  uint16_t y = (uint16_t)o.y;
  uint32_t base = breakable_color_for_hp(hp);

  if (renderer_draw_rectangle(x, y, TILE_SIZE, TILE_SIZE, base) != 0) return 1;
  if (renderer_draw_hline(x, y, TILE_SIZE, BREAKABLE_HIGHLIGHT) != 0) return 1;
  if (renderer_draw_rectangle(x, y, 1, TILE_SIZE, BREAKABLE_HIGHLIGHT) != 0) return 1;
  if (renderer_draw_hline(x, (uint16_t)(y + TILE_SIZE - 1), TILE_SIZE, BREAKABLE_SHADOW) != 0) return 1;
  if (renderer_draw_rectangle((uint16_t)(x + TILE_SIZE - 1), y, 1, TILE_SIZE, BREAKABLE_SHADOW) != 0) return 1;

  uint16_t mid_y = (uint16_t)(y + TILE_SIZE / 2);
  uint16_t mid_x = (uint16_t)(x + TILE_SIZE / 2);
  if (renderer_draw_hline(x, mid_y, TILE_SIZE, BREAKABLE_SHADOW) != 0) return 1;
  if (renderer_draw_rectangle(mid_x, y, 1, TILE_SIZE, BREAKABLE_SHADOW) != 0) return 1;
  return 0;
}

static int draw_teleporter(int row, int col, uint32_t frame_counter) {
  const Position o = tile_origin(row, col);
  uint16_t x = (uint16_t)o.x;
  uint16_t y = (uint16_t)o.y;
  uint32_t frame_index = (frame_counter / 6) % TELEPORTER_FRAME_COUNT;

  return sprite_draw(&teleporter_frames[frame_index], x, y);
}

int arena_view_load_assets(void) {
  for (int i = 0; i < TELEPORTER_FRAME_COUNT; i++) {
    sprite_init(&teleporter_frames[i]);
  }

  if (sprite_load(&teleporter_frames[0], teleporter_0_xpm) != 0) goto fail;
  if (sprite_load(&teleporter_frames[1], teleporter_1_xpm) != 0) goto fail;
  if (sprite_load(&teleporter_frames[2], teleporter_2_xpm) != 0) goto fail;
  if (sprite_load(&teleporter_frames[3], teleporter_3_xpm) != 0) goto fail;

  return 0;

fail:
  arena_view_destroy_assets();
  return 1;
}

void arena_view_destroy_assets(void) {
  for (int i = 0; i < TELEPORTER_FRAME_COUNT; i++) {
    sprite_destroy(&teleporter_frames[i]);
  }
}

int arena_view_draw(const Arena *arena, uint32_t frame_counter) {
  if (arena == NULL) return 1;

  for (int row = 0; row < ARENA_ROWS; row++) {
    for (int col = 0; col < ARENA_COLS; col++) {
      if (draw_floor(row, col) != 0) return 1;
      TileType type = arena->tiles[row][col].type;
      if (type == TILE_WALL) {
        if (draw_wall(row, col) != 0) return 1;
      } else if (type == TILE_BREAKABLE_WALL) {
        if (draw_breakable_wall(row, col, arena->tiles[row][col].hp) != 0) return 1;
      } else if (type == TILE_TELEPORTER_A || type == TILE_TELEPORTER_B) {
        if (draw_teleporter(row, col, frame_counter) != 0) return 1;
      }
    }
  }

  return 0;
}
