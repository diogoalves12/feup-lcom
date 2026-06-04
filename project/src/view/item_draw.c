#include "item_draw.h"

#include <stddef.h>

#include "arena.h"
#include "renderer.h"
#include "sprite.h"

#include "xpm/items/health_pickup_0.xpm"
#include "xpm/items/health_pickup_1.xpm"
#include "xpm/items/health_pickup_2.xpm"
#include "xpm/items/health_pickup_3.xpm"

static Sprite health_frames[HEALTH_PICKUP_FRAME_COUNT];
static bool   assets_loaded = false;

int item_view_load_assets(void) {
  for (int i = 0; i < HEALTH_PICKUP_FRAME_COUNT; i++) {
    sprite_init(&health_frames[i]);
  }

  if (sprite_load(&health_frames[0], health_pickup_0_xpm) != 0) goto fail;
  if (sprite_load(&health_frames[1], health_pickup_1_xpm) != 0) goto fail;
  if (sprite_load(&health_frames[2], health_pickup_2_xpm) != 0) goto fail;
  if (sprite_load(&health_frames[3], health_pickup_3_xpm) != 0) goto fail;

  assets_loaded = true;
  return 0;

fail:
  item_view_destroy_assets();
  return 1;
}

void item_view_destroy_assets(void) {
  for (int i = 0; i < HEALTH_PICKUP_FRAME_COUNT; i++) {
    sprite_destroy(&health_frames[i]);
  }
  assets_loaded = false;
}

int item_view_draw(const ItemManager *manager, uint32_t frame_counter) {
  if (manager == NULL) return 1;

  uint32_t frame_index = (frame_counter / 8) % HEALTH_PICKUP_FRAME_COUNT;

  for (int i = 0; i < manager->count; i++) {
    const Item *item = &manager->items[i];
    if (!item->active) continue;

    int draw_x = item->position.x - TILE_SIZE / 2;
    int draw_y = item->position.y - TILE_SIZE / 2;
    if (draw_x < 0 || draw_y < 0) continue;

    if (item->type == ITEM_HEALTH) {
      if (assets_loaded) {
        if (sprite_draw(&health_frames[frame_index], (uint16_t) draw_x, (uint16_t) draw_y) != 0)
          return 1;
      } else {
        if (renderer_draw_rectangle((uint16_t)(draw_x + 4), (uint16_t)(draw_y + 4),
                                    TILE_SIZE - 8, TILE_SIZE - 8, 0xE04040) != 0) return 1;
      }
    }
  }

  return 0;
}
