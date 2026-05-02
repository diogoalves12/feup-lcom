// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Any header files included below this line should have been created by you
#include "../lab3/i8042.h"
#include "../lab3/kbc.h"
#include "video.h"

#define XPM_MODE 0x105

static int (video_wait_for_esc)() {
    uint8_t bit_no;
    if (kbc_subscribe_int(&bit_no) != 0)
        return 1;

    uint32_t irq_set = BIT(bit_no);
    int ipc_status;
    int r;
    message msg;
    bool done = false;

    while (!done) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d\n", r);
            continue;
        }

        if (is_ipc_notify(ipc_status) && _ENDPOINT_P(msg.m_source) == HARDWARE) {
            if (msg.m_notify.interrupts & irq_set) {
                kbc_ih();

                if (!kbc_get_error() && kbc_get_byte() == ESC_BREAKCODE)
                    done = true;
            }
        }
    }

    if (kbc_unsubscribe_int() != 0)
        return 1;

    return 0;
}

static int (video_draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    xpm_image_t img;
    uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);
    if (pixmap == NULL)
        return 1;

    for (uint16_t row = 0; row < img.height; row++) {
        for (uint16_t col = 0; col < img.width; col++) {
            uint32_t color = pixmap[row * img.width + col];
            if (vg_draw_pixel(x + col, y + row, color) != 0) {
                free(pixmap);
                return 1;
            }
        }
    }

    free(pixmap);
    return 0;
}

int main(int argc, char *argv[]) {
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // enables to log function invocations that are being "wrapped" by LCF
    // [comment this out if you don't want/need it]
    lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    lcf_log_output("/home/lcom/labs/lab5/output.txt");

    // handles control over to LCF
    // [LCF handles command line arguments and invokes the right function]
    if (lcf_start(argc, argv))
        return 1;

    // LCF clean up tasks
    // [must be the last statement before return]
    lcf_cleanup();

    return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
    if (video_set_mode(mode) != 0)
        return 1;

    sleep(delay);

    if (vg_exit() != 0)
        return 1;

    return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
    if (video_map_vram(mode) != 0)
        return 1;

    if (video_set_mode(mode) != 0)
        return 1;

    if (vg_draw_rectangle(x, y, width, height, color) != 0)
        return 1;

    if (video_wait_for_esc() != 0)
        return 1;

    if (vg_exit() != 0)
        return 1;

    return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    if (video_map_vram(XPM_MODE) != 0)
        return 1;

    if (video_set_mode(XPM_MODE) != 0)
        return 1;

    if (video_draw_xpm(xpm, x, y) != 0)
        return 1;

    if (video_wait_for_esc() != 0)
        return 1;

    if (vg_exit() != 0)
        return 1;

    return 0;
}
