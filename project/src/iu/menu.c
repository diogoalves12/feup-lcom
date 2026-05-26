#include <lcom/lcf.h>
#include <stdint.h>
#include <stdbool.h>

#include "video.h"
#include "keyboard.h"
#include "mouse.h"
#include "menu.h"

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

typedef enum {
  MENU_RUNNING,
  MENU_START_GAME,
  MENU_EXIT_GAME
} MenuState;

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
  vg_draw_rectangle(button->x, button->y, button->w, button->h, color);
}

static void draw_menu(void) {
  vg_draw_rectangle(0, 0, SCREEN_W, SCREEN_H, COLOR_BG);

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

  swap_buffers(); // se o teu video.c usar double buffering
}

MenuState menu_update_keyboard(uint8_t scancode) {
  switch (scancode) {
    case 0x48: // seta cima
      selected_option = MENU_OPTION_PLAY;
      break;

    case 0x50: // seta baixo
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

  while (state == MENU_RUNNING) {
    draw_menu();

    /*
     * Aqui deves chamar o teu loop de interrupções:
     *
     * - teclado
     * - rato
     * - timer
     *
     * Quando receberes scancode:
     * state = menu_update_keyboard(scancode);
     *
     * Quando receberes packet do rato:
     * state = menu_update_mouse(mouse_x, mouse_y, left_click);
     */

  }

  return state;

}