#ifndef _LCOM_LAB5_VIDEO_H_
#define _LCOM_LAB5_VIDEO_H_

#include <lcom/lcf.h>

#include <stdint.h>

int (video_set_mode)(uint16_t mode);
int (video_map_vram)(uint16_t mode);
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

#endif /* _LCOM_LAB5_VIDEO_H_ */
