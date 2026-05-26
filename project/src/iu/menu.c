#include <lcom/lcf.h>
#include <stdint.h>
#include <stdbool.h>

#include "video.h"
#include "keyboard.h"
#include "mouse.h"
#include "menu.h"
#include "renderer.h"

#define MENU_OPTION_PLAY 0
#define MENU_OPTION_EXIT 1

#define SCREEN_W 800
#define SCREEN_H 600

#define BUTTON_W 220
#define BUTTON_H 70

#define PLAY_X ((SCREEN_W - BUTTON_W) / 2)
#define PLAY_Y 220

#define EXIT_X ((SCREEN_W - BUTTON_W) / 2)
#define EXIT_Y 330

#define COLOR_BG        0x1E1E1E
#define COLOR_BUTTON    0x444444
#define COLOR_HOVER     0x777777
#define COLOR_SELECTED  0x00AAFF
#define COLOR_TEXT      0xFFFFFF



typedef struct {
  int x;
  int y;
  int w;
  int h;
} Button;

static Button play_button = { PLAY_X, PLAY_Y, BUTTON_W, BUTTON_H };
static Button exit_button = { EXIT_X, EXIT_Y, BUTTON_W, BUTTON_H };

static int selected_option = MENU_OPTION_PLAY;

static bool point_inside_button(int x, int y, Button *button) {
  return x >= button->x &&
         x <= button->x + button->w &&
         y >= button->y &&
         y <= button->y + button->h;
}

static void draw_button(Button *button, uint32_t color) {
  renderer_draw_rectangle(button->x, button->y, button->w, button->h, color);
}

static void draw_menu(void) {
  renderer_clear(COLOR_BG);

  if (selected_option == MENU_OPTION_PLAY) {
    draw_button(&play_button, COLOR_SELECTED);
    draw_button(&exit_button, COLOR_BUTTON);
  } else {
    draw_button(&play_button, COLOR_BUTTON);
    draw_button(&exit_button, COLOR_SELECTED);
  }

  /*
   * Se já tiveres função para desenhar texto, podes usar aqui:
   *
   * draw_text("PLAY", PLAY_X + 75, PLAY_Y + 25, COLOR_TEXT);
   * draw_text("EXIT", EXIT_X + 75, EXIT_Y + 25, COLOR_TEXT);
   *
   * Caso ainda não tenhas texto, substitui isto por sprites XPM.
   */

  renderer_present();
}

MenuState menu_update_keyboard(uint8_t scancode) {
  switch (scancode) {
    case 0x48: // seta cima
    case 0x11: // W
      selected_option = MENU_OPTION_PLAY;
      break;

    case 0x50: // seta baixo
    case 0x1F: // S
      selected_option = MENU_OPTION_EXIT;
      break;

    case 0x1C: // ENTER
      if (selected_option == MENU_OPTION_PLAY)
        return MENU_START_GAME;
      else
        return MENU_EXIT_GAME;

    case 0x01: // ESC
      return MENU_EXIT_GAME;
  }

  return MENU_RUNNING;
}

MenuState menu_update_mouse(int mouse_x, int mouse_y, bool left_click) {
  if (point_inside_button(mouse_x, mouse_y, &play_button)) {
    selected_option = MENU_OPTION_PLAY;

    if (left_click)
      return MENU_START_GAME;
  }

  if (point_inside_button(mouse_x, mouse_y, &exit_button)) {
    selected_option = MENU_OPTION_EXIT;

    if (left_click)
      return MENU_EXIT_GAME;
  }

  return MENU_RUNNING;
}

MenuState menu_loop(void) {
  MenuState state = MENU_RUNNING;

  uint8_t keyboard_bit_no;
  if (keyboard_subscribe_int(&keyboard_bit_no) != 0) {
    return MENU_EXIT_GAME;
  }

  int ipc_status;
  message msg;
  bool should_draw = true;

  while (state == MENU_RUNNING) {
    if (should_draw) {
      draw_menu();
      should_draw = false;
    }

    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & BIT(keyboard_bit_no)) {
          kbc_ih();
          if (!keyboard_has_error()) {
            uint8_t scancode = keyboard_get_scancode();
            if (scancode != 0) {
              state = menu_update_keyboard(scancode);
              should_draw = true;
            }
          }
        }
      }
    }
  }

  keyboard_unsubscribe_int();
  return state;
}
