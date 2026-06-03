#include "pause_menu.h"

#include <stddef.h>

#include "renderer.h"

#define PAUSE_OPTION_COLOR   0x404040
#define PAUSE_SELECTED_COLOR 0x00AAFF

#define PAUSE_OPTION_WIDTH  200
#define PAUSE_OPTION_HEIGHT  54
#define PAUSE_OPTION_X      300

#define PAUSE_CONTINUE_Y  210
#define PAUSE_RESTART_Y   278
#define PAUSE_MAIN_MENU_Y 346

void pause_menu_state_init(PauseMenuState *menu) {
  if (menu == NULL) return;
  menu->selection = PAUSE_SEL_CONTINUE;
}

void pause_menu_state_update(PauseMenuState *menu, const GameInputActions *actions,
                             GameState *next) {
  if (menu == NULL || actions == NULL || next == NULL) return;

  if (actions->nav_up && menu->selection > PAUSE_SEL_CONTINUE) {
    menu->selection = (PauseSelection)(menu->selection - 1);
  }
  if (actions->nav_down && menu->selection < PAUSE_SEL_MAIN_MENU) {
    menu->selection = (PauseSelection)(menu->selection + 1);
  }

  if (actions->confirm) {
    switch (menu->selection) {
      case PAUSE_SEL_CONTINUE:  *next = GAME_STATE_PLAYING; break;
      case PAUSE_SEL_RESTART:   *next = GAME_STATE_PLAYING; break;
      case PAUSE_SEL_MAIN_MENU: *next = GAME_STATE_MENU;    break;
    }
    return;
  }

  if (actions->pause_requested) {
    *next = GAME_STATE_PLAYING;
    return;
  }

  if (actions->back) {
    *next = GAME_STATE_MENU;
  }
}

int pause_menu_state_render(const PauseMenuState *menu) {
  if (menu == NULL) return 1;

  if (renderer_draw_rectangle(PAUSE_OPTION_X, PAUSE_CONTINUE_Y,
                              PAUSE_OPTION_WIDTH, PAUSE_OPTION_HEIGHT,
                              menu->selection == PAUSE_SEL_CONTINUE
                                ? PAUSE_SELECTED_COLOR : PAUSE_OPTION_COLOR) != 0) {
    return 1;
  }

  if (renderer_draw_rectangle(PAUSE_OPTION_X, PAUSE_RESTART_Y,
                              PAUSE_OPTION_WIDTH, PAUSE_OPTION_HEIGHT,
                              menu->selection == PAUSE_SEL_RESTART
                                ? PAUSE_SELECTED_COLOR : PAUSE_OPTION_COLOR) != 0) {
    return 1;
  }

  if (renderer_draw_rectangle(PAUSE_OPTION_X, PAUSE_MAIN_MENU_Y,
                              PAUSE_OPTION_WIDTH, PAUSE_OPTION_HEIGHT,
                              menu->selection == PAUSE_SEL_MAIN_MENU
                                ? PAUSE_SELECTED_COLOR : PAUSE_OPTION_COLOR) != 0) {
    return 1;
  }

  return 0;
}
