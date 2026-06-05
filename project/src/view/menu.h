#ifndef PROJECT_UI_MENU_H
#define PROJECT_UI_MENU_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "game_input.h"
#include "game_state.h"
#include "mouse_input.h"
#include "sprite.h"

typedef enum {
  MENU_SCREEN_MAIN = 0,
  MENU_SCREEN_DIFFICULTY
} MenuScreen;

typedef enum {
  MENU_SEL_START = 0,
  MENU_SEL_DIFFICULTY,
  MENU_SEL_LOG,
  MENU_SEL_EXIT
} MenuSelection;

typedef enum {
  MENU_DIFF_SEL_EASY = 0,
  MENU_DIFF_SEL_MEDIUM,
  MENU_DIFF_SEL_HARD,
  MENU_DIFF_SEL_BACK
} MenuDiffSelection;

typedef struct {
  MenuScreen        screen;
  MenuSelection     selection;
  MenuDiffSelection diff_selection;
  ArenaDifficulty   selected_difficulty;
  Sprite            button;
  Sprite            button_selected;
  Sprite            start_text;
  Sprite            difficulty_text;
  Sprite            log_text;
  Sprite            exit_text;
  Sprite            select_diff_title;
  Sprite            easy_text;
  Sprite            medium_text;
  Sprite            hard_text;
  Sprite            back_text;
  Sprite            cursor;
  int16_t           cursor_x;
  int16_t           cursor_y;
  bool              prev_lb;
  bool              assets_loaded;
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
