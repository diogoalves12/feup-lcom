#include "pause_menu.h"

#include <stddef.h>

#include "config.h"
#include "renderer.h"

#include "xpm/buttons/button_gray_wide.xpm"
#include "xpm/buttons/button_red_wide.xpm"
#include "xpm/text/pause.xpm"
#include "xpm/text/resume.xpm"
#include "xpm/text/retry.xpm"
#include "xpm/text/back.xpm"

#define PAUSE_OPTION_COLOR   0x404040
#define PAUSE_SELECTED_COLOR 0xAA0000

#define PAUSE_BTN_W   560
#define PAUSE_BTN_H    90

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
  menu->assets_loaded = false;
}

int pause_menu_state_load_assets(PauseMenuState *menu) {
  if (menu == NULL) return 1;

  if (sprite_load(&menu->button,          button_gray_wide)  != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->button_selected, button_red_wide)   != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->title,           pause_title_xpm)   != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->resume_text,     resume_xpm)        != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->retry_text,      retry_xpm)         != 0) { pause_menu_state_destroy_assets(menu); return 1; }
  if (sprite_load(&menu->back_text,       back)              != 0) { pause_menu_state_destroy_assets(menu); return 1; }

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
  menu->assets_loaded = false;
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

static int draw_pause_button(const PauseMenuState *menu, bool selected,
                             const Sprite *label, uint16_t slot_y) {
  const Sprite *btn = selected ? &menu->button_selected : &menu->button;
  uint16_t bx = (uint16_t)((SCREEN_WIDTH  - (int)btn->width)  / 2);
  uint16_t by = (uint16_t)(slot_y + ((int)PAUSE_BTN_H - (int)btn->height) / 2);
  if (sprite_draw(btn, bx, by) != 0) return 1;

  if (label->loaded) {
    int lx = ((int)SCREEN_WIDTH - (int)label->width)  / 2;
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
                        &menu->retry_text,  PAUSE_RETRY_Y)  != 0) return 1;
  if (draw_pause_button(menu, menu->selection == PAUSE_SEL_MAIN_MENU,
                        &menu->back_text,   PAUSE_BACK_Y)   != 0) return 1;
  return 0;
}

static int render_fallback(const PauseMenuState *menu) {
  int bx = (SCREEN_WIDTH - PAUSE_BTN_W) / 2;

  if (renderer_draw_rectangle((uint16_t)bx, PAUSE_RESUME_Y, PAUSE_BTN_W, PAUSE_BTN_H,
        menu->selection == PAUSE_SEL_CONTINUE  ? PAUSE_SELECTED_COLOR : PAUSE_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle((uint16_t)bx, PAUSE_RETRY_Y,  PAUSE_BTN_W, PAUSE_BTN_H,
        menu->selection == PAUSE_SEL_RESTART   ? PAUSE_SELECTED_COLOR : PAUSE_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle((uint16_t)bx, PAUSE_BACK_Y,   PAUSE_BTN_W, PAUSE_BTN_H,
        menu->selection == PAUSE_SEL_MAIN_MENU ? PAUSE_SELECTED_COLOR : PAUSE_OPTION_COLOR) != 0) return 1;
  return 0;
}

int pause_menu_state_render(const PauseMenuState *menu) {
  if (menu == NULL) return 1;

  return menu->assets_loaded ? render_sprites(menu) : render_fallback(menu);
}
