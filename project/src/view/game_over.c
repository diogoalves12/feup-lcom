#include "game_over.h"

#include <stddef.h>

#include "config.h"
#include "renderer.h"

#include "xpm/buttons/button_gray_wide.xpm"
#include "xpm/buttons/button_red_wide.xpm"
#include "xpm/text/game_over.xpm"
#include "xpm/text/player1_wins.xpm"
#include "xpm/text/player2_wins.xpm"
#include "xpm/text/retry.xpm"
#include "xpm/text/back.xpm"
#include "xpm/text/exit.xpm"

#define GAME_OVER_BG_COLOR       0x101010
#define GAME_OVER_OPTION_COLOR   0x404040
#define GAME_OVER_SELECTED_COLOR 0xAA0000

#define GAME_OVER_BTN_W   560
#define GAME_OVER_BTN_H    90

#define GAME_OVER_TITLE_Y    30
#define GAME_OVER_WINNER_Y  130
#define GAME_OVER_RETRY_Y   240
#define GAME_OVER_BACK_Y    350
#define GAME_OVER_EXIT_Y    460

void game_over_state_init(GameOverState *state, int winner) {
  if (state == NULL) return;
  state->winner    = winner;
  state->selection = GAME_OVER_SEL_RESTART;
  sprite_init(&state->button);
  sprite_init(&state->button_selected);
  sprite_init(&state->title);
  sprite_init(&state->player1_wins_text);
  sprite_init(&state->player2_wins_text);
  sprite_init(&state->retry_text);
  sprite_init(&state->back_text);
  sprite_init(&state->exit_text);
  state->assets_loaded = false;
}

int game_over_state_load_assets(GameOverState *state) {
  if (state == NULL) return 1;

  if (sprite_load(&state->button,           button_gray_wide)   != 0) { game_over_state_destroy_assets(state); return 1; }
  if (sprite_load(&state->button_selected,  button_red_wide)    != 0) { game_over_state_destroy_assets(state); return 1; }
  if (sprite_load(&state->title,            game_over_xpm)      != 0) { game_over_state_destroy_assets(state); return 1; }
  if (sprite_load(&state->player1_wins_text,player1_wins_xpm)   != 0) { game_over_state_destroy_assets(state); return 1; }
  if (sprite_load(&state->player2_wins_text,player2_wins_xpm)   != 0) { game_over_state_destroy_assets(state); return 1; }
  if (sprite_load(&state->retry_text,       retry_xpm)          != 0) { game_over_state_destroy_assets(state); return 1; }
  if (sprite_load(&state->back_text,        back)               != 0) { game_over_state_destroy_assets(state); return 1; }
  if (sprite_load(&state->exit_text,        exit_label_xpm)     != 0) { game_over_state_destroy_assets(state); return 1; }

  state->assets_loaded = true;
  return 0;
}

void game_over_state_destroy_assets(GameOverState *state) {
  if (state == NULL) return;
  sprite_destroy(&state->button);
  sprite_destroy(&state->button_selected);
  sprite_destroy(&state->title);
  sprite_destroy(&state->player1_wins_text);
  sprite_destroy(&state->player2_wins_text);
  sprite_destroy(&state->retry_text);
  sprite_destroy(&state->back_text);
  sprite_destroy(&state->exit_text);
  state->assets_loaded = false;
}

void game_over_state_update(GameOverState *state, const GameInputActions *actions, GameState *next) {
  if (state == NULL || actions == NULL || next == NULL) return;

  if (actions->nav_up && state->selection > GAME_OVER_SEL_RESTART) {
    state->selection = (GameOverSelection)(state->selection - 1);
  }
  if (actions->nav_down && state->selection < GAME_OVER_SEL_EXIT) {
    state->selection = (GameOverSelection)(state->selection + 1);
  }

  if (actions->confirm) {
    switch (state->selection) {
      case GAME_OVER_SEL_RESTART: *next = GAME_STATE_PLAYING;  break;
      case GAME_OVER_SEL_MENU:    *next = GAME_STATE_MENU;     break;
      case GAME_OVER_SEL_EXIT:    *next = GAME_STATE_EXIT;     break;
    }
  } else if (actions->back) {
    *next = GAME_STATE_EXIT;
  }
}

static int draw_game_over_button(const GameOverState *state, bool selected,
                                 const Sprite *label, uint16_t slot_y) {
  const Sprite *btn = selected ? &state->button_selected : &state->button;
  uint16_t bx = (uint16_t)((SCREEN_WIDTH  - (int)btn->width)  / 2);
  uint16_t by = (uint16_t)(slot_y + ((int)GAME_OVER_BTN_H - (int)btn->height) / 2);
  if (sprite_draw(btn, bx, by) != 0) return 1;

  if (label->loaded) {
    int lx = ((int)SCREEN_WIDTH - (int)label->width)  / 2;
    int ly = (int)slot_y + ((int)GAME_OVER_BTN_H - (int)label->height) / 2;
    if (lx < 0) lx = 0;
    if (ly < 0) ly = 0;
    if (sprite_draw(label, (uint16_t)lx, (uint16_t)ly) != 0) return 1;
  }
  return 0;
}

static int render_sprites(const GameOverState *state) {
  if (renderer_clear(GAME_OVER_BG_COLOR) != 0) return 1;

  if (state->title.loaded) {
    int tx = ((int)SCREEN_WIDTH - (int)state->title.width) / 2;
    if (tx < 0) tx = 0;
    if (sprite_draw(&state->title, (uint16_t)tx, GAME_OVER_TITLE_Y) != 0) return 1;
  }

  const Sprite *winner_label = (state->winner == 1)
    ? &state->player1_wins_text
    : &state->player2_wins_text;
  if (winner_label->loaded) {
    int wx = ((int)SCREEN_WIDTH - (int)winner_label->width) / 2;
    if (wx < 0) wx = 0;
    if (sprite_draw(winner_label, (uint16_t)wx, GAME_OVER_WINNER_Y) != 0) return 1;
  }

  if (draw_game_over_button(state, state->selection == GAME_OVER_SEL_RESTART,
                            &state->retry_text, GAME_OVER_RETRY_Y) != 0) return 1;
  if (draw_game_over_button(state, state->selection == GAME_OVER_SEL_MENU,
                            &state->back_text,  GAME_OVER_BACK_Y)  != 0) return 1;
  if (draw_game_over_button(state, state->selection == GAME_OVER_SEL_EXIT,
                            &state->exit_text,  GAME_OVER_EXIT_Y)  != 0) return 1;

  return renderer_present();
}

static int render_fallback(const GameOverState *state) {
  if (renderer_clear(GAME_OVER_BG_COLOR) != 0) return 1;

  int bx = (SCREEN_WIDTH - GAME_OVER_BTN_W) / 2;

  if (renderer_draw_rectangle((uint16_t)bx, GAME_OVER_RETRY_Y, GAME_OVER_BTN_W, GAME_OVER_BTN_H,
        state->selection == GAME_OVER_SEL_RESTART ? GAME_OVER_SELECTED_COLOR : GAME_OVER_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle((uint16_t)bx, GAME_OVER_BACK_Y,  GAME_OVER_BTN_W, GAME_OVER_BTN_H,
        state->selection == GAME_OVER_SEL_MENU    ? GAME_OVER_SELECTED_COLOR : GAME_OVER_OPTION_COLOR) != 0) return 1;
  if (renderer_draw_rectangle((uint16_t)bx, GAME_OVER_EXIT_Y,  GAME_OVER_BTN_W, GAME_OVER_BTN_H,
        state->selection == GAME_OVER_SEL_EXIT    ? GAME_OVER_SELECTED_COLOR : GAME_OVER_OPTION_COLOR) != 0) return 1;

  return renderer_present();
}

int game_over_state_render(const GameOverState *state) {
  if (state == NULL) return 1;

  return state->assets_loaded ? render_sprites(state) : render_fallback(state);
}
