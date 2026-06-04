#ifndef PROJECT_UI_PAUSE_MENU_H
#define PROJECT_UI_PAUSE_MENU_H

#include <stdbool.h>
#include <stdint.h>

#include "game_input.h"
#include "game_state.h"
#include "mouse_input.h"
#include "sprite.h"

typedef enum {
  PAUSE_SEL_CONTINUE = 0,
  PAUSE_SEL_RESTART = 1,
  PAUSE_SEL_MAIN_MENU = 2
} PauseSelection;

typedef struct {
  PauseSelection selection;
  Sprite         button;
  Sprite         button_selected;
  Sprite         title;
  Sprite         resume_text;
  Sprite         retry_text;
  Sprite         back_text;
  Sprite         cursor;
  int16_t        cursor_x;
  int16_t        cursor_y;
  bool           prev_lb;
  bool           assets_loaded;
} PauseMenuState;

void pause_menu_state_init(PauseMenuState *menu);
int  pause_menu_state_load_assets(PauseMenuState *menu);
void pause_menu_state_destroy_assets(PauseMenuState *menu);
void pause_menu_state_reset_cursor(PauseMenuState *menu);
void pause_menu_state_move_cursor(PauseMenuState *menu, int16_t dx, int16_t dy);
void pause_menu_state_apply_mouse(PauseMenuState *menu, const MouseInput *mouse, GameInputActions *actions);
void pause_menu_state_update(PauseMenuState *menu, const GameInputActions *actions,
                             GameState *next);
int  pause_menu_state_render(const PauseMenuState *menu);

#endif
