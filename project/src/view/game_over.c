#include "game_over.h"

#include <stddef.h>

#include "renderer.h"

#define GAME_OVER_BG_COLOR       0x101010
#define GAME_OVER_OPTION_COLOR   0x404040
#define GAME_OVER_SELECTED_COLOR 0x00AAFF
#define GAME_OVER_P1_COLOR       0x00AAFF
#define GAME_OVER_P2_COLOR       0xFF4040

#define GAME_OVER_OPTION_WIDTH   260
#define GAME_OVER_OPTION_X       270

/* Vertical layout: winner bar, then three options. */
#define GAME_OVER_WINNER_Y   80
#define GAME_OVER_WINNER_H   60
#define GAME_OVER_RESTART_Y  200
#define GAME_OVER_MENU_Y     280
#define GAME_OVER_EXIT_Y     360
#define GAME_OVER_OPTION_H   60

void game_over_state_init(GameOverState *state, int winner) {
  if (state == NULL) return;
  state->winner    = winner;
  state->selection = GAME_OVER_SEL_RESTART;
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

int game_over_state_render(const GameOverState *state) {
  if (state == NULL) return 1;

  if (renderer_clear(GAME_OVER_BG_COLOR) != 0) return 1;

  /* Winner bar: colored by the winning player. */
  uint32_t winner_color = (state->winner == 1) ? GAME_OVER_P1_COLOR : GAME_OVER_P2_COLOR;
  if (renderer_draw_rectangle(GAME_OVER_OPTION_X, GAME_OVER_WINNER_Y,
                              GAME_OVER_OPTION_WIDTH, GAME_OVER_WINNER_H,
                              winner_color) != 0) {
    return 1;
  }

  /* Restart */
  if (renderer_draw_rectangle(GAME_OVER_OPTION_X, GAME_OVER_RESTART_Y,
                              GAME_OVER_OPTION_WIDTH, GAME_OVER_OPTION_H,
                              state->selection == GAME_OVER_SEL_RESTART
                                ? GAME_OVER_SELECTED_COLOR : GAME_OVER_OPTION_COLOR) != 0) {
    return 1;
  }

  /* Menu */
  if (renderer_draw_rectangle(GAME_OVER_OPTION_X, GAME_OVER_MENU_Y,
                              GAME_OVER_OPTION_WIDTH, GAME_OVER_OPTION_H,
                              state->selection == GAME_OVER_SEL_MENU
                                ? GAME_OVER_SELECTED_COLOR : GAME_OVER_OPTION_COLOR) != 0) {
    return 1;
  }

  /* Exit */
  if (renderer_draw_rectangle(GAME_OVER_OPTION_X, GAME_OVER_EXIT_Y,
                              GAME_OVER_OPTION_WIDTH, GAME_OVER_OPTION_H,
                              state->selection == GAME_OVER_SEL_EXIT
                                ? GAME_OVER_SELECTED_COLOR : GAME_OVER_OPTION_COLOR) != 0) {
    return 1;
  }

  return renderer_present();
}
