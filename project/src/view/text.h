#ifndef PROJECT_VIEW_TEXT_H
#define PROJECT_VIEW_TEXT_H

#include <stdint.h>

#define TEXT_GLYPH_W 5
#define TEXT_GLYPH_H 7

int  text_char_width(uint8_t scale);
int  text_string_width(const char *str, uint8_t scale);
void text_draw(int x, int y, const char *str, uint8_t scale, uint32_t color);

#endif
