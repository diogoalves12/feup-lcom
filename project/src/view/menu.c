#include "menu.h"

#include <stddef.h>

#include "config.h"
#include "renderer.h"

#include "xpm/text/exit.xpm"

#include "xpm/buttons/button_gray_wide.xpm"
#include "xpm/buttons/button_red_wide.xpm"
#include "xpm/cursor/pointer_b_shaded.xpm"
#include "xpm/text/start.xpm"
#include "xpm/text/difficulty.xpm"
#include "xpm/text/select_difficulty.xpm"
#include "xpm/text/easy.xpm"
#include "xpm/text/medium.xpm"
#include "xpm/text/hard.xpm"
#include "xpm/text/back.xpm"

#define MENU_BG_COLOR       0x101010
#define MENU_OPTION_COLOR   0x404040
#define MENU_SELECTED_COLOR 0x00AAFF

#define MENU_BTN_W  560
#define MENU_BTN_H   90
#define MENU_BTN_X  120
#define MENU_START_Y      145
#define MENU_DIFFICULTY_Y 255
#define MENU_EXIT_Y       365
#define DIFF_TITLE_Y   60
#define DIFF_EASY_Y    135
#define DIFF_MEDIUM_Y  240
#define DIFF_HARD_Y    345
#define DIFF_BACK_Y    450

static void menu_state_select_main_start(MenuState *menu) {
  menu->screen = MENU_SCREEN_MAIN;
  menu->selection = MENU_SEL_START;
  menu->cursor_x = SCREEN_WIDTH / 2;
  menu->cursor_y = MENU_START_Y + MENU_BTN_H / 2;
}

void menu_state_init(MenuState *menu) {
  if (menu == NULL) return;
  menu_state_select_main_start(menu);
  menu->diff_selection = MENU_DIFF_SEL_MEDIUM;
  menu->selected_difficulty = DEFAULT_ARENA_DIFFICULTY;
  sprite_init(&menu->button);
  sprite_init(&menu->button_selected);
  sprite_init(&menu->start_text);
  sprite_init(&menu->difficulty_text);
  sprite_init(&menu->exit_text);
  sprite_init(&menu->select_diff_title);
  sprite_init(&menu->easy_text);
  sprite_init(&menu->medium_text);
  sprite_init(&menu->hard_text);
  sprite_init(&menu->back_text);
  sprite_init(&menu->cursor);
  menu->prev_lb = false;
  menu->assets_loaded = false;
}

int menu_state_load_assets(MenuState *menu) {
  if (menu == NULL) return 1;

  if (sprite_load(&menu->button, button_gray_wide) != 0) { menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->button_selected, button_red_wide) != 0) { menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->start_text, start) != 0) { menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->difficulty_text, difficulty) != 0) { menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->exit_text, exit_label_xpm) != 0) { menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->select_diff_title, select_difficulty) != 0) { menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->easy_text, easy) != 0) { menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->medium_text, medium) != 0) { menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->hard_text, hard) != 0) { menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->back_text, back) != 0) { menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->cursor, pointer_b_shaded_xpm) != 0) { menu_state_destroy_assets(menu); return 1; }

  menu->assets_loaded = true;
  return 0;
}

void menu_state_destroy_assets(MenuState *menu) {
  if (menu == NULL) return;
  sprite_destroy(&menu->button);
  sprite_destroy(&menu->button_selected);
  sprite_destroy(&menu->start_text);
  sprite_destroy(&menu->difficulty_text);
  sprite_destroy(&menu->exit_text);
  sprite_destroy(&menu->select_diff_title);
  sprite_destroy(&menu->easy_text);
  sprite_destroy(&menu->medium_text);
  sprite_destroy(&menu->hard_text);
  sprite_destroy(&menu->back_text);
  sprite_destroy(&menu->cursor);
  menu->assets_loaded = false;
}

void menu_state_reset(MenuState *menu) {
  if (menu == NULL) return;
  menu_state_select_main_start(menu);
  menu->diff_selection = (MenuDiffSelection)menu->selected_difficulty;
  menu->prev_lb = false;
}

static void menu_state_update_hover(MenuState *menu) {
  int cx = (int)menu->cursor_x;
  int cy = (int)menu->cursor_y;

  if (cx < MENU_BTN_X || cx >= MENU_BTN_X + MENU_BTN_W) return;

  if (menu->screen == MENU_SCREEN_MAIN) {
    if (cy >= MENU_START_Y && cy < MENU_START_Y + MENU_BTN_H) menu->selection = MENU_SEL_START;
    else if (cy >= MENU_DIFFICULTY_Y && cy < MENU_DIFFICULTY_Y + MENU_BTN_H) menu->selection = MENU_SEL_DIFFICULTY;
    else if (cy >= MENU_EXIT_Y && cy < MENU_EXIT_Y + MENU_BTN_H) menu->selection = MENU_SEL_EXIT;
  } else {
    if (cy >= DIFF_EASY_Y && cy < DIFF_EASY_Y + MENU_BTN_H) menu->diff_selection = MENU_DIFF_SEL_EASY;
    else if (cy >= DIFF_MEDIUM_Y && cy < DIFF_MEDIUM_Y + MENU_BTN_H) menu->diff_selection = MENU_DIFF_SEL_MEDIUM;
    else if (cy >= DIFF_HARD_Y && cy < DIFF_HARD_Y + MENU_BTN_H) menu->diff_selection = MENU_DIFF_SEL_HARD;
    else if (cy >= DIFF_BACK_Y && cy < DIFF_BACK_Y + MENU_BTN_H) menu->diff_selection = MENU_DIFF_SEL_BACK;
  }
}

void menu_state_move_cursor(MenuState *menu, int16_t dx, int16_t dy) {
  if (menu == NULL) return;

  int cur_w = (menu->cursor.loaded && menu->cursor.width > 0) ? (int)menu->cursor.width : 1;
  int cur_h = (menu->cursor.loaded && menu->cursor.height > 0) ? (int)menu->cursor.height : 1;
  int max_x = SCREEN_WIDTH - cur_w;
  int max_y = SCREEN_HEIGHT - cur_h;

  int candidate_x = (int)menu->cursor_x + (int)dx;
  int candidate_y = (int)menu->cursor_y - (int)dy;

  if (candidate_x < 0) candidate_x = 0;
  else if (candidate_x > max_x) candidate_x = max_x;

  if (candidate_y < 0) candidate_y = 0;
  else if (candidate_y > max_y) candidate_y = max_y;

  menu->cursor_x = (int16_t)candidate_x;
  menu->cursor_y = (int16_t)candidate_y;

  menu_state_update_hover(menu);
}

void menu_state_apply_mouse(MenuState *menu, const MouseInput *mouse, GameInputActions *actions) {
  if (menu == NULL || mouse == NULL || actions == NULL) return;
  menu_state_update_hover(menu);
  if (mouse->move_forward && !menu->prev_lb)
    actions->confirm = true;
  menu->prev_lb = mouse->move_forward;
}

void menu_state_update(MenuState *menu, const GameInputActions *actions, GameState *next) {
  if (menu == NULL || actions == NULL || next == NULL) return;

  if (menu->screen == MENU_SCREEN_MAIN) {
    if (actions->nav_up && menu->selection > MENU_SEL_START)
      menu->selection = (MenuSelection)(menu->selection - 1);
    if (actions->nav_down && menu->selection < MENU_SEL_EXIT)
      menu->selection = (MenuSelection)(menu->selection + 1);

    if (actions->confirm) {
      switch (menu->selection) {
        case MENU_SEL_START: *next = GAME_STATE_PLAYING; break;
        case MENU_SEL_DIFFICULTY: menu->screen = MENU_SCREEN_DIFFICULTY; break;
        case MENU_SEL_EXIT: *next = GAME_STATE_EXIT; break;
      }
    } else if (actions->back) {
      *next = GAME_STATE_EXIT;
    }
  } else {
    if (actions->nav_up && menu->diff_selection > MENU_DIFF_SEL_EASY)
      menu->diff_selection = (MenuDiffSelection)(menu->diff_selection - 1);
    if (actions->nav_down && menu->diff_selection < MENU_DIFF_SEL_BACK)
      menu->diff_selection = (MenuDiffSelection)(menu->diff_selection + 1);

    if (actions->confirm) {
      switch (menu->diff_selection) {
        case MENU_DIFF_SEL_EASY:
          menu->selected_difficulty = ARENA_EASY;
          menu_state_select_main_start(menu);
          break;
        case MENU_DIFF_SEL_MEDIUM:
          menu->selected_difficulty = ARENA_MEDIUM;
          menu_state_select_main_start(menu);
          break;
        case MENU_DIFF_SEL_HARD:
          menu->selected_difficulty = ARENA_HARD;
          menu_state_select_main_start(menu);
          break;
        case MENU_DIFF_SEL_BACK:
          menu_state_select_main_start(menu);
          break;
      }
    } else if (actions->back) {
      menu_state_select_main_start(menu);
    }
  }
}

static int draw_button(const MenuState *menu, bool selected,
                       const Sprite *label, uint16_t slot_y) {
  const Sprite *btn = selected ? &menu->button_selected : &menu->button;
  uint16_t bx = (uint16_t)((SCREEN_WIDTH - (int)btn->width) / 2);
  uint16_t by = (uint16_t)(slot_y + ((int)MENU_BTN_H - (int)btn->height) / 2);
  if (sprite_draw(btn, bx, by) != 0) return 1;

  if (label->loaded) {
    int lx = ((int)SCREEN_WIDTH - (int)label->width) / 2;
    int ly = (int)slot_y + ((int)MENU_BTN_H - (int)label->height) / 2;
    if (lx < 0) lx = 0;
    if (ly < 0) ly = 0;
    if (sprite_draw(label, (uint16_t)lx, (uint16_t)ly) != 0) return 1;
  }
  return 0;
}

static int render_main_screen_sprites(const MenuState *menu) {
  if (renderer_clear(MENU_BG_COLOR) != 0) return 1;

  if (draw_button(menu, menu->selection == MENU_SEL_START, &menu->start_text, MENU_START_Y) != 0) return 1;
  if (draw_button(menu, menu->selection == MENU_SEL_DIFFICULTY, &menu->difficulty_text, MENU_DIFFICULTY_Y) != 0) return 1;
  if (draw_button(menu, menu->selection == MENU_SEL_EXIT, &menu->exit_text, MENU_EXIT_Y) != 0) return 1;

  sprite_draw_clipped(&menu->cursor, menu->cursor_x, menu->cursor_y);
  return renderer_present();
}

static int render_difficulty_screen_sprites(const MenuState *menu) {
  if (renderer_clear(MENU_BG_COLOR) != 0) return 1;

  if (menu->select_diff_title.loaded) {
    int tx = ((int)SCREEN_WIDTH - (int)menu->select_diff_title.width) / 2;
    if (tx < 0) tx = 0;
    if (sprite_draw(&menu->select_diff_title, (uint16_t)tx, DIFF_TITLE_Y) != 0) return 1;
  }

  if (draw_button(menu, menu->diff_selection == MENU_DIFF_SEL_EASY, &menu->easy_text, DIFF_EASY_Y) != 0) return 1;
  if (draw_button(menu, menu->diff_selection == MENU_DIFF_SEL_MEDIUM, &menu->medium_text, DIFF_MEDIUM_Y) != 0) return 1;
  if (draw_button(menu, menu->diff_selection == MENU_DIFF_SEL_HARD, &menu->hard_text, DIFF_HARD_Y) != 0) return 1;
  if (draw_button(menu, menu->diff_selection == MENU_DIFF_SEL_BACK, &menu->back_text, DIFF_BACK_Y) != 0) return 1;

  sprite_draw_clipped(&menu->cursor, menu->cursor_x, menu->cursor_y);
  return renderer_present();
}

static int render_main_screen_fallback(const MenuState *menu) {
  if (renderer_clear(MENU_BG_COLOR) != 0) return 1;

  if (renderer_draw_rectangle(MENU_BTN_X, MENU_START_Y, MENU_BTN_W, MENU_BTN_H,
        menu->selection == MENU_SEL_START ? MENU_SELECTED_COLOR : MENU_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle(MENU_BTN_X, MENU_DIFFICULTY_Y, MENU_BTN_W, MENU_BTN_H,
        menu->selection == MENU_SEL_DIFFICULTY ? MENU_SELECTED_COLOR : MENU_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle(MENU_BTN_X, MENU_EXIT_Y, MENU_BTN_W, MENU_BTN_H,
        menu->selection == MENU_SEL_EXIT ? MENU_SELECTED_COLOR : MENU_OPTION_COLOR) != 0) return 1;

  return renderer_present();
}

static int render_difficulty_screen_fallback(const MenuState *menu) {
  if (renderer_clear(MENU_BG_COLOR) != 0) return 1;

  if (renderer_draw_rectangle(MENU_BTN_X, DIFF_EASY_Y, MENU_BTN_W, MENU_BTN_H,
        menu->diff_selection == MENU_DIFF_SEL_EASY ? MENU_SELECTED_COLOR : MENU_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle(MENU_BTN_X, DIFF_MEDIUM_Y, MENU_BTN_W, MENU_BTN_H,
        menu->diff_selection == MENU_DIFF_SEL_MEDIUM ? MENU_SELECTED_COLOR : MENU_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle(MENU_BTN_X, DIFF_HARD_Y, MENU_BTN_W, MENU_BTN_H,
        menu->diff_selection == MENU_DIFF_SEL_HARD ? MENU_SELECTED_COLOR : MENU_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle(MENU_BTN_X, DIFF_BACK_Y, MENU_BTN_W, MENU_BTN_H,
        menu->diff_selection == MENU_DIFF_SEL_BACK ? MENU_SELECTED_COLOR : MENU_OPTION_COLOR) != 0) return 1;

  return renderer_present();
}

int menu_state_render(const MenuState *menu) {
  if (menu == NULL) return 1;

  if (menu->screen == MENU_SCREEN_MAIN) {
    return menu->assets_loaded
      ? render_main_screen_sprites(menu)
      : render_main_screen_fallback(menu);
  } else {
    return menu->assets_loaded
      ? render_difficulty_screen_sprites(menu)
      : render_difficulty_screen_fallback(menu);
  }
}
