/**
 * @file sprite.h
 * @brief XPM sprite loading and drawing helpers.
 *
 * A Sprite owns decoded XPM pixel data. Drawing skips the transparent color.
 * sprite_destroy() frees the pixel buffer but not the struct itself.
 */
#ifndef SPRITE_H
#define SPRITE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <lcom/xpm.h>

/**
 * @brief Decoded XPM image.
 */
typedef struct {
  uint16_t width;             /**< Image width in pixels. */
  uint16_t height;            /**< Image height in pixels. */
  uint8_t *pixels;            /**< Heap-allocated pixel data (24-bit BGR). */
  uint32_t transparent_color; /**< Pixels with this color are not drawn. */
  bool     loaded;            /**< True once sprite_load() has succeeded. */
} Sprite;

/**
 * @brief Sets the sprite to an unloaded state.
 */
void sprite_init(Sprite *sprite);

/**
 * @brief Loads an XPM map into a Sprite.
 */
int sprite_load(Sprite *sprite, xpm_map_t xpm);

/**
 * @brief Draws the sprite at (x, y), skipping transparent pixels.
 *
 * Assumes the sprite fits entirely within the framebuffer.
 */
int sprite_draw(const Sprite *sprite, uint16_t x, uint16_t y);

/**
 * @brief Draws the sprite with screen edge clipping.
 *
 * Safely handles negative coordinates or positions that extend beyond
 * the framebuffer.
 */
int sprite_draw_clipped(const Sprite *sprite, int16_t x, int16_t y);

/**
 * @brief Draws the sprite centered at (cx, cy) rotated by @p angle radians.
 *
 * Used for player and bullet sprites that must follow a facing direction.
 */
int sprite_draw_rotated(const Sprite *sprite, int cx, int cy, float angle);

/**
 * @brief Frees sprite pixels and marks the sprite as unloaded.
 */
void sprite_destroy(Sprite *sprite);

#endif
