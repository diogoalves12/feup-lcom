#ifndef PROJECT_GRAPHICS_RENDERER_H
#define PROJECT_GRAPHICS_RENDERER_H

#include <stdint.h>

int renderer_init(uint16_t mode);
int renderer_clear(uint32_t color);
int renderer_draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int renderer_shutdown(void);

#endif
