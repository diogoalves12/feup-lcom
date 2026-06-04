#ifndef PROJECT_VIEW_SPRITE_H
#define PROJECT_VIEW_SPRITE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <lcom/xpm.h>

typedef struct {
  uint16_t width;
  uint16_t height;
  uint8_t *pixels;
  uint32_t transparent_color;
  bool loaded;
} Sprite;

void sprite_init(Sprite *sprite);
int  sprite_load(Sprite *sprite, xpm_map_t xpm);
int  sprite_draw(const Sprite *sprite, uint16_t x, uint16_t y);
int  sprite_draw_clipped(const Sprite *sprite, int16_t x, int16_t y);
int  sprite_draw_rotated(const Sprite *sprite, int cx, int cy, float angle);
void sprite_destroy(Sprite *sprite);

#endif
