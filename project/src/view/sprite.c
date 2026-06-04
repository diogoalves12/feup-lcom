#include "sprite.h"

#include <stdlib.h>
#include <lcom/lcf.h>

#include "renderer.h"

void sprite_init(Sprite *sprite) {
  if (sprite == NULL) return;
  sprite->width             = 0;
  sprite->height            = 0;
  sprite->pixels            = NULL;
  sprite->transparent_color = 0;
  sprite->loaded            = false;
}

void sprite_destroy(Sprite *sprite) {
  if (sprite == NULL) return;
  if (sprite->pixels != NULL) {
    free(sprite->pixels);
    sprite->pixels = NULL;
  }
  sprite->width             = 0;
  sprite->height            = 0;
  sprite->transparent_color = 0;
  sprite->loaded            = false;
}

int sprite_load(Sprite *sprite, xpm_map_t xpm) {
  if (sprite == NULL || xpm == NULL) return 1;
  if (sprite->loaded) sprite_destroy(sprite);
  enum xpm_image_type type = XPM_8_8_8_8;
  xpm_image_t img;
  uint8_t *pixels = xpm_load(xpm, type, &img);
  if (pixels == NULL) return 1;
  sprite->width             = img.width;
  sprite->height            = img.height;
  sprite->pixels            = pixels;
  sprite->transparent_color = xpm_transparency_color(type);
  sprite->loaded            = true;
  return 0;
}

int sprite_draw(const Sprite *sprite, uint16_t x, uint16_t y) {
  if (sprite == NULL || !sprite->loaded || sprite->pixels == NULL) return 1;
  for (uint16_t row = 0; row < sprite->height; row++) {
    for (uint16_t col = 0; col < sprite->width; col++) {
      size_t offset = ((size_t) row * sprite->width + col) * 4;
      uint32_t color = 0;
      color |= (uint32_t) sprite->pixels[offset];
      color |= (uint32_t) sprite->pixels[offset + 1] << 8;
      color |= (uint32_t) sprite->pixels[offset + 2] << 16;
      color |= (uint32_t) sprite->pixels[offset + 3] << 24;
      if (color == sprite->transparent_color) continue;
      if (renderer_draw_pixel(x + col, y + row, color) != 0) return 1;
    }
  }
  return 0;
}
