// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#if __has_include(<machine/int86.h>)
#include <machine/int86.h>
#elif __has_include("../.minix-src/include/machine/int86.h")
#include "../.minix-src/include/machine/int86.h"
#endif

#if __has_include(<sys/errno.h>)
#include <sys/errno.h>
#elif __has_include("../.minix-src/include/sys/errno.h")
#include "../.minix-src/include/sys/errno.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>

#include "video.h"

#ifndef OK
#define OK 0
#endif

#define VBE_SET_MODE 0x4F02
#define VBE_LINEAR_FRAME_BUFFER BIT(14)
#define BIOS_VIDEO_SERVICE 0x10

static char *video_mem;
static vbe_mode_info_t vmi;
static unsigned bytes_per_pixel;

int (video_set_mode)(uint16_t mode) {
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));

  reg.intno = BIOS_VIDEO_SERVICE;
  reg.ax = VBE_SET_MODE;
  reg.bx = mode | VBE_LINEAR_FRAME_BUFFER;

  if (sys_int86(&reg) != OK) {
    printf("%s: sys_int86 failed\n", __func__);
    return 1;
  }

  if (reg.ah != 0x00) {
    printf("%s: VBE call failed, AH=0x%02x\n", __func__, reg.ah);
    return 1;
  }

  return 0;
}

int (video_map_vram)(uint16_t mode) {
  if (vbe_get_mode_info(mode, &vmi) != 0) return 1;

  bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;
  unsigned int vram_base = vmi.PhysBasePtr;
  unsigned int vram_size = vmi.XResolution * vmi.YResolution * bytes_per_pixel;

  struct minix_mem_range mr;
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK) return 1;

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED) return 1;

  return 0;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if (video_mem == NULL) return 1;
  if (x >= vmi.XResolution || y >= vmi.YResolution) return 1;

  unsigned int offset = (y * vmi.XResolution + x) * bytes_per_pixel;
  memcpy(video_mem + offset, &color, bytes_per_pixel);

  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (uint16_t i = 0; i < len; i++) {
    if (vg_draw_pixel(x + i, y, color) != 0) return 1;
  }

  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (uint16_t i = 0; i < height; i++) {
    if (vg_draw_hline(x, y + i, width, color) != 0) return 1;
  }

  return 0;
}
