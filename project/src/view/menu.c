#include "menu.h"

#include <stddef.h>

#include "renderer.h"

#define MENU_BG_COLOR       0x101010
#define MENU_OPTION_COLOR   0x404040
#define MENU_SELECTED_COLOR 0x00AAFF

#define MENU_OPTION_WIDTH   260
#define MENU_OPTION_HEIGHT  80
#define MENU_OPTION_X       270
#define MENU_START_Y        200
#define MENU_EXIT_Y         320

void menu_state_init(MenuState *menu) {
  if (menu == NULL) return;
  menu->selection = MENU_SEL_START;
}

void menu_state_update(MenuState *menu, const GameInputActions *actions, GameState *next) {
  if (menu == NULL || actions == NULL || next == NULL) return;

  if (actions->nav_up && menu->selection > MENU_SEL_START) {
    menu->selection = (MenuSelection)(menu->selection - 1);
  }
  if (actions->nav_down && menu->selection < MENU_SEL_EXIT) {
    menu->selection = (MenuSelection)(menu->selection + 1);
  }

  if (actions->confirm) {
    *next = (menu->selection == MENU_SEL_START) ? GAME_STATE_PLAYING : GAME_STATE_EXIT;
  } else if (actions->back) {
    *next = GAME_STATE_EXIT;
  }
}

int menu_state_render(const MenuState *menu) {
  if (menu == NULL) return 1;

  if (renderer_clear(MENU_BG_COLOR) != 0) return 1;

  if (renderer_draw_rectangle(MENU_OPTION_X, MENU_START_Y,
                              MENU_OPTION_WIDTH, MENU_OPTION_HEIGHT,
                              menu->selection == MENU_SEL_START
                                ? MENU_SELECTED_COLOR : MENU_OPTION_COLOR) != 0) {
    return 1;
  }

  if (renderer_draw_rectangle(MENU_OPTION_X, MENU_EXIT_Y,
                              MENU_OPTION_WIDTH, MENU_OPTION_HEIGHT,
                              menu->selection == MENU_SEL_EXIT
                                ? MENU_SELECTED_COLOR : MENU_OPTION_COLOR) != 0) {
    return 1;
  }

  return renderer_present();
}
