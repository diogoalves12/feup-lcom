#ifndef PROJECT_UI_MENU_H
#define PROJECT_UI_MENU_H

#include <stdint.h>

#include "game_input.h"
#include "game_state.h"
#include "mouse_input.h"
#include "sprite.h"

typedef enum {
  MENU_SEL_START = 0,
  MENU_SEL_EXIT
} MenuSelection;

typedef struct {
  MenuSelection selection;
  Sprite        button;
  Sprite        button_selected;
  Sprite        play_text;
  Sprite        cursor;
  int16_t       cursor_x;
  int16_t       cursor_y;
  bool          prev_lb;
  bool          assets_loaded;
} MenuState;

void menu_state_init(MenuState *menu);
void menu_state_reset(MenuState *menu);
void menu_state_move_cursor(MenuState *menu, int16_t dx, int16_t dy);
void menu_state_apply_mouse(MenuState *menu, const MouseInput *mouse, GameInputActions *actions);
void menu_state_update(MenuState *menu, const GameInputActions *actions, GameState *next);
int  menu_state_render(const MenuState *menu);
int  menu_state_load_assets(MenuState *menu);
void menu_state_destroy_assets(MenuState *menu);

#endif
