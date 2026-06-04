#include "menu.h"

#include <stddef.h>

#include "renderer.h"

#include "xpm/menu_button.xpm"
#include "xpm/menu_button_selected.xpm"
#include "xpm/menu_play_text.xpm"

#define MENU_BG_COLOR       0x101010
#define MENU_OPTION_COLOR   0x404040
#define MENU_SELECTED_COLOR 0x00AAFF

#define MENU_OPTION_WIDTH   260
#define MENU_OPTION_HEIGHT  80
#define MENU_OPTION_X       270
#define MENU_START_Y        250
#define MENU_EXIT_Y         300

void menu_state_init(MenuState *menu) {
  if (menu == NULL) return;
  menu->selection = MENU_SEL_START;
  sprite_init(&menu->button);
  sprite_init(&menu->button_selected);
  sprite_init(&menu->play_text);
  menu->assets_loaded = false;
}

int menu_state_load_assets(MenuState *menu) {
  if (menu == NULL) return 1;
  if (sprite_load(&menu->button, menu_button_xpm) != 0) return 1;
  if (sprite_load(&menu->button_selected, menu_button_selected_xpm) != 0) {
    sprite_destroy(&menu->button);
    return 1;
  }
  if (sprite_load(&menu->play_text, menu_play_text_xpm) != 0) {
    sprite_destroy(&menu->button_selected);
    sprite_destroy(&menu->button);
    return 1;
  }
  menu->assets_loaded = true;
  return 0;
}

void menu_state_destroy_assets(MenuState *menu) {
  if (menu == NULL) return;
  sprite_destroy(&menu->button);
  sprite_destroy(&menu->button_selected);
  sprite_destroy(&menu->play_text);
  menu->assets_loaded = false;
}

void menu_state_reset(MenuState *menu) {
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

static int render_with_sprites(const MenuState *menu) {
  if (renderer_clear(MENU_BG_COLOR) != 0) return 1;

  const Sprite *start_btn = (menu->selection == MENU_SEL_START) ? &menu->button_selected : &menu->button;
  const Sprite *exit_btn  = (menu->selection == MENU_SEL_EXIT)  ? &menu->button_selected : &menu->button;

  uint16_t btn_x   = (uint16_t)(MENU_OPTION_X + (MENU_OPTION_WIDTH  - (int) start_btn->width)  / 2);
  uint16_t start_y = (uint16_t)(MENU_START_Y   + (MENU_OPTION_HEIGHT - (int) start_btn->height) / 2);
  uint16_t exit_y  = (uint16_t)(MENU_EXIT_Y    + (MENU_OPTION_HEIGHT - (int) exit_btn->height)  / 2);
  uint16_t txt_x   = (uint16_t)(btn_x   + (int)(start_btn->width  - menu->play_text.width)  / 2);
  uint16_t txt_y   = (uint16_t)(start_y + (int)(start_btn->height - menu->play_text.height) / 2);

  if (sprite_draw(start_btn,      btn_x, start_y) != 0) return 1;
  if (sprite_draw(&menu->play_text, txt_x, txt_y) != 0) return 1;
  if (sprite_draw(exit_btn,       btn_x, exit_y)  != 0) return 1;

  return renderer_present();
}

int menu_state_render(const MenuState *menu) {
  if (menu == NULL) return 1;

  if (menu->assets_loaded) return render_with_sprites(menu);

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
