#ifndef PROJECT_UI_MENU_H
#define PROJECT_UI_MENU_H

#include "game_input.h"
#include "game_state.h"

typedef enum {
  MENU_SEL_START = 0,
  MENU_SEL_EXIT
} MenuSelection;

typedef struct {
  MenuSelection selection;
} MenuState;

void menu_state_init(MenuState *menu);
void menu_state_update(MenuState *menu, const GameInputActions *actions, GameState *next);
int menu_state_render(const MenuState *menu);

#endif
