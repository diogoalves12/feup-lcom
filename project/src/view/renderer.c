#include "renderer.h"

#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "video.h"

static bool renderer_is_initialized = false;
static uint8_t *renderer_vram = NULL;
static uint8_t *renderer_back_buffer = NULL;
static uint16_t renderer_width = 0;
static uint16_t renderer_height = 0;
static uint8_t renderer_bytes_per_pixel = 0;
static size_t renderer_frame_size = 0;

static bool renderer_can_draw(void) {
  return renderer_is_initialized && renderer_back_buffer != NULL;
}

static void renderer_reset_state(void) {
  renderer_is_initialized = false;
  renderer_vram = NULL;
  renderer_back_buffer = NULL;
  renderer_width = 0;
  renderer_height = 0;
  renderer_bytes_per_pixel = 0;
  renderer_frame_size = 0;
}

static int renderer_configure_mode(uint16_t mode) {
  vbe_mode_info_t mode_info;

  if (vbe_get_mode_info(mode, &mode_info) != 0) {
    printf("%s: failed to get VBE mode info for 0x%03X\n", __func__, mode);
    return 1;
  }

  renderer_width = mode_info.XResolution;
  renderer_height = mode_info.YResolution;
  renderer_bytes_per_pixel = (mode_info.BitsPerPixel + 7) / 8;
  renderer_frame_size = (size_t) renderer_width * renderer_height * renderer_bytes_per_pixel;

  struct minix_mem_range memory_range;
  memory_range.mr_base = (phys_bytes) mode_info.PhysBasePtr;
  memory_range.mr_limit = memory_range.mr_base + renderer_frame_size;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &memory_range) != 0) {
    printf("%s: sys_privctl failed\n", __func__);
    return 1;
  }

  renderer_vram = vm_map_phys(SELF, (void *) memory_range.mr_base, renderer_frame_size);
  if (renderer_vram == MAP_FAILED) {
    renderer_vram = NULL;
    printf("%s: vm_map_phys failed\n", __func__);
    return 1;
  }

  return 0;
}

int renderer_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (!renderer_can_draw()) {
    return 1;
  }

  if (x >= renderer_width || y >= renderer_height) {
    return 1;
  }

  size_t offset = ((size_t) y * renderer_width + x) * renderer_bytes_per_pixel;
  memcpy(renderer_back_buffer + offset, &color, renderer_bytes_per_pixel);

  return 0;
}

int renderer_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  if (!renderer_can_draw()) {
    return 1;
  }

  for (uint16_t column = 0; column < len; column++) {
    if (renderer_draw_pixel(x + column, y, color) != 0) {
      return 1;
    }
  }

  return 0;
}

int renderer_init(uint16_t mode) {
  renderer_reset_state();

  if (renderer_configure_mode(mode) != 0) {
    return 1;
  }

  if (video_set_mode(mode) != 0) {
    printf("%s: failed to enter graphics mode\n", __func__);
    renderer_reset_state();
    return 1;
  }

  renderer_back_buffer = malloc(renderer_frame_size);
  if (renderer_back_buffer == NULL) {
    printf("%s: failed to allocate hidden buffer\n", __func__);
    vg_exit();
    renderer_reset_state();
    return 1;
  }

  memset(renderer_back_buffer, 0, renderer_frame_size);
  renderer_is_initialized = true;

  return 0;
}

int renderer_clear(uint32_t color) {
  if (!renderer_can_draw()) {
    return 1;
  }

  if (renderer_bytes_per_pixel == 1) {
    memset(renderer_back_buffer, (uint8_t) color, renderer_frame_size);
    return 0;
  }

  for (size_t offset = 0; offset < renderer_frame_size; offset += renderer_bytes_per_pixel) {
    memcpy(renderer_back_buffer + offset, &color, renderer_bytes_per_pixel);
  }

  return 0;
}

int renderer_draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  if (!renderer_can_draw()) {
    return 1;
  }

  for (uint16_t row = 0; row < height; row++) {
    if (renderer_draw_hline(x, y + row, width, color) != 0) {
      return 1;
    }
  }

  return 0;
}

int renderer_present(void) {
  if (!renderer_is_initialized || renderer_vram == NULL || renderer_back_buffer == NULL) {
    return 1;
  }

  memcpy(renderer_vram, renderer_back_buffer, renderer_frame_size);
  return 0;
}

int renderer_shutdown(void) {
  int result = 0;

  if (renderer_is_initialized && vg_exit() != 0) {
    printf("%s: failed to return to text mode\n", __func__);
    result = 1;
  }

  if (renderer_back_buffer != NULL) {
    free(renderer_back_buffer);
    renderer_back_buffer = NULL;
  }

  renderer_reset_state();
  return result;
}
