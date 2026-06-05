#include "pause_menu.h"

#include <stddef.h>

#include "config.h"
#include "renderer.h"

#include "xpm/buttons/button_gray_wide.xpm"
#include "xpm/buttons/button_red_wide.xpm"
#include "xpm/cursor/pointer_b_shaded.xpm"
#include "xpm/text/pause.xpm"
#include "xpm/text/resume.xpm"
#include "xpm/text/retry.xpm"
#include "xpm/text/main_menu.xpm"

#define PAUSE_OPTION_COLOR   0x404040
#define PAUSE_SELECTED_COLOR 0xAA0000

#define PAUSE_BTN_W   560
#define PAUSE_BTN_H    90
#define PAUSE_BTN_X   ((SCREEN_WIDTH - PAUSE_BTN_W) / 2)

#define PAUSE_TITLE_Y  80
#define PAUSE_RESUME_Y 190
#define PAUSE_RETRY_Y  300
#define PAUSE_BACK_Y   410

void pause_menu_state_init(PauseMenuState *menu) {
  if (menu == NULL) return;
  menu->selection = PAUSE_SEL_CONTINUE;
  sprite_init(&menu->button);
  sprite_init(&menu->button_selected);
  sprite_init(&menu->title);
  sprite_init(&menu->resume_text);
  sprite_init(&menu->retry_text);
  sprite_init(&menu->back_text);
  sprite_init(&menu->cursor);
  menu->cursor_x = SCREEN_WIDTH / 2;
  menu->cursor_y = PAUSE_RESUME_Y + PAUSE_BTN_H / 2;
  menu->prev_lb = false;
  menu->assets_loaded = false;
}

int pause_menu_state_load_assets(PauseMenuState *menu) {
  if (menu == NULL) return 1;

  if (sprite_load(&menu->button, button_gray_wide) != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->button_selected, button_red_wide) != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->title, pause_title_xpm) != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->resume_text, resume_xpm) != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->retry_text, retry_xpm) != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->back_text, main_menu) != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->cursor, pointer_b_shaded_xpm) != 0) { pause_menu_state_destroy_assets(menu); return 1; }

  menu->assets_loaded = true;
  return 0;
}

void pause_menu_state_destroy_assets(PauseMenuState *menu) {
  if (menu == NULL) return;
  sprite_destroy(&menu->button);
  sprite_destroy(&menu->button_selected);
  sprite_destroy(&menu->title);
  sprite_destroy(&menu->resume_text);
  sprite_destroy(&menu->retry_text);
  sprite_destroy(&menu->back_text);
  sprite_destroy(&menu->cursor);
  menu->assets_loaded = false;
}

static void pause_update_hover(PauseMenuState *menu) {
  int cx = (int)menu->cursor_x;
  int cy = (int)menu->cursor_y;

  if (cx < PAUSE_BTN_X || cx >= PAUSE_BTN_X + PAUSE_BTN_W) return;

  if (cy >= PAUSE_RESUME_Y && cy < PAUSE_RESUME_Y + PAUSE_BTN_H) menu->selection = PAUSE_SEL_CONTINUE;
  else if (cy >= PAUSE_RETRY_Y && cy < PAUSE_RETRY_Y + PAUSE_BTN_H) menu->selection = PAUSE_SEL_RESTART;
  else if (cy >= PAUSE_BACK_Y && cy < PAUSE_BACK_Y + PAUSE_BTN_H) menu->selection = PAUSE_SEL_MAIN_MENU;
}

void pause_menu_state_reset_cursor(PauseMenuState *menu) {
  if (menu == NULL) return;
  menu->cursor_x = SCREEN_WIDTH / 2;
  menu->cursor_y = PAUSE_RESUME_Y + PAUSE_BTN_H / 2;
  pause_update_hover(menu);
}

void pause_menu_state_move_cursor(PauseMenuState *menu, int16_t dx, int16_t dy) {
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

  pause_update_hover(menu);
}

void pause_menu_state_apply_mouse(PauseMenuState *menu, const MouseInput *mouse, GameInputActions *actions) {
  if (menu == NULL || mouse == NULL || actions == NULL) return;
  pause_update_hover(menu);
  if (mouse->move_forward && !menu->prev_lb)
    actions->confirm = true;
  menu->prev_lb = mouse->move_forward;
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
      case PAUSE_SEL_CONTINUE: *next = GAME_STATE_PLAYING; break;
      case PAUSE_SEL_RESTART: *next = GAME_STATE_PLAYING; break;
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

static int draw_pause_button(const PauseMenuState *menu, bool selected,
                             const Sprite *label, uint16_t slot_y) {
  const Sprite *btn = selected ? &menu->button_selected : &menu->button;
  uint16_t bx = (uint16_t)((SCREEN_WIDTH - (int)btn->width) / 2);
  uint16_t by = (uint16_t)(slot_y + ((int)PAUSE_BTN_H - (int)btn->height) / 2);
  if (sprite_draw(btn, bx, by) != 0) return 1;

  if (label->loaded) {
    int lx = ((int)SCREEN_WIDTH - (int)label->width) / 2;
    int ly = (int)slot_y + ((int)PAUSE_BTN_H - (int)label->height) / 2;
    if (lx < 0) lx = 0;
    if (ly < 0) ly = 0;
    if (sprite_draw(label, (uint16_t)lx, (uint16_t)ly) != 0) return 1;
  }
  return 0;
}

static int render_sprites(const PauseMenuState *menu) {
  if (menu->title.loaded) {
    int tx = ((int)SCREEN_WIDTH - (int)menu->title.width) / 2;
    if (tx < 0) tx = 0;
    if (sprite_draw(&menu->title, (uint16_t)tx, PAUSE_TITLE_Y) != 0) return 1;
  }

  if (draw_pause_button(menu, menu->selection == PAUSE_SEL_CONTINUE,
                        &menu->resume_text, PAUSE_RESUME_Y) != 0) return 1;
  if (draw_pause_button(menu, menu->selection == PAUSE_SEL_RESTART,
                        &menu->retry_text, PAUSE_RETRY_Y) != 0) return 1;
  if (draw_pause_button(menu, menu->selection == PAUSE_SEL_MAIN_MENU,
                        &menu->back_text, PAUSE_BACK_Y) != 0) return 1;

  sprite_draw_clipped(&menu->cursor, menu->cursor_x, menu->cursor_y);
  return 0;
}

static int render_fallback(const PauseMenuState *menu) {
  int bx = (SCREEN_WIDTH - PAUSE_BTN_W) / 2;

  if (renderer_draw_rectangle((uint16_t)bx, PAUSE_RESUME_Y, PAUSE_BTN_W, PAUSE_BTN_H,
        menu->selection == PAUSE_SEL_CONTINUE ? PAUSE_SELECTED_COLOR : PAUSE_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle((uint16_t)bx, PAUSE_RETRY_Y, PAUSE_BTN_W, PAUSE_BTN_H,
        menu->selection == PAUSE_SEL_RESTART ? PAUSE_SELECTED_COLOR : PAUSE_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle((uint16_t)bx, PAUSE_BACK_Y, PAUSE_BTN_W, PAUSE_BTN_H,
        menu->selection == PAUSE_SEL_MAIN_MENU ? PAUSE_SELECTED_COLOR : PAUSE_OPTION_COLOR) != 0) return 1;

  sprite_draw_clipped(&menu->cursor, menu->cursor_x, menu->cursor_y);
  return 0;
}

int pause_menu_state_render(const PauseMenuState *menu) {
  if (menu == NULL) return 1;

  return menu->assets_loaded ? render_sprites(menu) : render_fallback(menu);
}
