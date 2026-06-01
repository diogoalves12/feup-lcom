#ifndef PROJECT_UI_PAUSE_MENU_H
#define PROJECT_UI_PAUSE_MENU_H

#include "game_input.h"
#include "game_state.h"

typedef enum {
  PAUSE_SEL_CONTINUE  = 0,
  PAUSE_SEL_RESTART   = 1,
  PAUSE_SEL_MAIN_MENU = 2
} PauseSelection;

typedef struct {
  PauseSelection selection;
} PauseMenuState;

void pause_menu_state_init(PauseMenuState *menu);
void pause_menu_state_update(PauseMenuState *menu, const GameInputActions *actions,
                             GameState *next);
int  pause_menu_state_render(const PauseMenuState *menu);

#endif
