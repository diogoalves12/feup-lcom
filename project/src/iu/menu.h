#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  MENU_RUNNING,
  MENU_START_GAME,
  MENU_EXIT_GAME
} MenuState;

MenuState menu_loop(void);

MenuState menu_update_keyboard(uint8_t scancode);

MenuState menu_update_mouse(int mouse_x, int mouse_y, bool left_click);

#endif