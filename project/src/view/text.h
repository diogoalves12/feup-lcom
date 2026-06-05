/**
 * @file text.h
 * @brief Small bitmap font renderer.
 *
 * Renders ASCII text with a built in 5x7 monospace font and integer scaling.
 */
#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>

#define TEXT_GLYPH_W 5 /**< Character width in pixels at scale 1. */
#define TEXT_GLYPH_H 7 /**< Character height in pixels at scale 1. */

/**
 * @brief Returns the width of one glyph at @p scale.
 */
int text_char_width(uint8_t scale);

/**
 * @brief Returns the width of a string at @p scale.
 */
int text_string_width(const char *str, uint8_t scale);

/**
 * @brief Draws an ASCII string at (x, y).
 */
void text_draw(int x, int y, const char *str, uint8_t scale, uint32_t color);

#endif
