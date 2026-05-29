#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#include "menu.h"
#include "keyboard.h"
#include "renderer.h"

#define MENU_BACKGROUND_COLOR 0x101010
#define MENU_OPTION_COLOR 0x404040
#define MENU_SELECTED_COLOR 0x00AAFF

#define MENU_OPTION_WIDTH 260
#define MENU_OPTION_HEIGHT 80
#define MENU_OPTION_X 270
#define MENU_START_OPTION_Y 200
#define MENU_EXIT_OPTION_Y 320

#define W_MAKECODE 0x11
#define S_MAKECODE 0x1F
#define SPACE_MAKECODE 0x39
#define ENTER_MAKECODE 0x1C
#define ESC_BREAKCODE 0x81

static int menu_draw(bool start_game_selected);
static MenuResult menu_result_from_selection(bool start_game_selected);

static int menu_draw(bool start_game_selected) {
  if (renderer_clear(MENU_BACKGROUND_COLOR) != 0) {
    return 1;
  }

  // top rectangle = Start Game
  if (renderer_draw_rectangle(MENU_OPTION_X, MENU_START_OPTION_Y,
                              MENU_OPTION_WIDTH, MENU_OPTION_HEIGHT,
                              start_game_selected ? MENU_SELECTED_COLOR : MENU_OPTION_COLOR) != 0) {
    return 1;
  }

  // bottom rectangle = Exit
  if (renderer_draw_rectangle(MENU_OPTION_X, MENU_EXIT_OPTION_Y,
                              MENU_OPTION_WIDTH, MENU_OPTION_HEIGHT,
                              start_game_selected ? MENU_OPTION_COLOR : MENU_SELECTED_COLOR) != 0) {
    return 1;
  }

  return renderer_present();
}

static MenuResult menu_result_from_selection(bool start_game_selected) {
  if (start_game_selected) {
    return MENU_RESULT_START_GAME;
  }

  return MENU_RESULT_EXIT_GAME;
}

MenuResult menu_loop(void) {
  uint8_t keyboard_bit_no;
  bool start_game_selected = true;
  MenuResult result = MENU_RESULT_EXIT_GAME;
  int ipc_status;
  message msg;

  if (keyboard_subscribe_int(&keyboard_bit_no) != 0) {
    return MENU_RESULT_EXIT_GAME;
  }

  if (menu_draw(start_game_selected) != 0) {
    keyboard_unsubscribe_int();
    return MENU_RESULT_EXIT_GAME;
  }

  while (true) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      break;
    }

    if (!is_ipc_notify(ipc_status)) {
      continue;
    }

    if (_ENDPOINT_P(msg.m_source) != HARDWARE) {
      continue;
    }

    if (!(msg.m_notify.interrupts & BIT(keyboard_bit_no))) {
      continue;
    }

    kbc_ih();

    if (keyboard_has_error()) {
      continue;
    }

    switch (keyboard_get_scancode()) {
      case W_MAKECODE:
        start_game_selected = true;
        if (menu_draw(start_game_selected) != 0) {
          keyboard_unsubscribe_int();
          return MENU_RESULT_EXIT_GAME;
        }
        break;
      case S_MAKECODE:
        start_game_selected = false;
        if (menu_draw(start_game_selected) != 0) {
          keyboard_unsubscribe_int();
          return MENU_RESULT_EXIT_GAME;
        }
        break;
      case SPACE_MAKECODE:
      case ENTER_MAKECODE:
        result = menu_result_from_selection(start_game_selected);
        keyboard_unsubscribe_int();
        return result;
      case ESC_BREAKCODE:
        keyboard_unsubscribe_int();
        return MENU_RESULT_EXIT_GAME;
      default:
        break;
    }
  }

  keyboard_unsubscribe_int();

  return MENU_RESULT_EXIT_GAME;
}
