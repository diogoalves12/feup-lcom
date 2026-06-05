/**
 * @file renderer.h
 * @brief Double-buffered renderer.
 *
 * Draw calls write to a hidden back buffer. renderer_present() copies that
 * buffer to VRAM once at the end of the frame.
 */
#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

/**
 * @brief Enters graphics mode and allocates the back buffer.
 */
int renderer_init(uint16_t mode);

/**
 * @brief Fills the back buffer with one color.
 */
int renderer_clear(uint32_t color);

/**
 * @brief Draws a single pixel to the back buffer.
 */
int renderer_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief Draws a horizontal line to the back buffer.
 */
int renderer_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * @brief Draws a filled rectangle to the back buffer.
 */
int renderer_draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

/**
 * @brief Copies the back buffer to VRAM, making the frame visible.
 *
 * Call exactly once per frame, after all draw calls are complete.
 */
int renderer_present(void);

/**
 * @brief Frees renderer memory and returns to text mode.
 */
int renderer_shutdown(void);

#endif
