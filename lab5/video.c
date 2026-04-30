// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <string.h>

#include "video.h"

#define VBE_SET_MODE 0x4F02
#define VBE_LINEAR_FRAME_BUFFER BIT(14)
#define BIOS_VIDEO_SERVICE 0x10

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
