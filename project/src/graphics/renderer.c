#include "renderer.h"

#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "video.h"

#define MODE_0X115 0x115
#define MODE_0X115_WIDTH 800
#define MODE_0X115_HEIGHT 600

static bool renderer_is_initialized = false;
static uint16_t renderer_width = 0;
static uint16_t renderer_height = 0;

static int renderer_set_dimensions(uint16_t mode) {
  switch (mode) {
    case MODE_0X115:
      renderer_width = MODE_0X115_WIDTH;
      renderer_height = MODE_0X115_HEIGHT;
      return 0;
    default:
      printf("%s: unsupported mode 0x%03X\n", __func__, mode);
      return 1;
  }
}

int renderer_init(uint16_t mode) {
  if (renderer_set_dimensions(mode) != 0) {
    return 1;
  }

  if (video_map_vram(mode) != 0) {
    printf("%s: failed to map VRAM\n", __func__);
    return 1;
  }

  if (video_set_mode(mode) != 0) {
    printf("%s: failed to enter graphics mode\n", __func__);
    return 1;
  }

  renderer_is_initialized = true;
  return 0;
}

int renderer_clear(uint32_t color) {
  if (!renderer_is_initialized) {
    return 1;
  }

  return vg_draw_rectangle(0, 0, renderer_width, renderer_height, color);
}

int renderer_draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  if (!renderer_is_initialized) {
    return 1;
  }

  return vg_draw_rectangle(x, y, width, height, color);
}

int renderer_draw_test_scene(void) {
  if (renderer_clear(0x101010) != 0) {
    printf("%s: failed to clear background\n", __func__);
    return 1;
  }

  if (renderer_draw_rectangle(80, 60, 220, 140, 0x0033CC) != 0) {
    printf("%s: failed to draw first rectangle\n", __func__);
    return 1;
  }

  if (renderer_draw_rectangle(420, 240, 320, 180, 0xCC5500) != 0) {
    printf("%s: failed to draw second rectangle\n", __func__);
    return 1;
  }

  if (renderer_draw_rectangle(180, 500, 500, 80, 0x33AA33) != 0) {
    printf("%s: failed to draw third rectangle\n", __func__);
    return 1;
  }

  return 0;
}

int renderer_shutdown(void) {
  if (!renderer_is_initialized) {
    return 0;
  }

  if (vg_exit() != 0) {
    printf("%s: failed to return to text mode\n", __func__);
    return 1;
  }

  renderer_is_initialized = false;
  return 0;
}
