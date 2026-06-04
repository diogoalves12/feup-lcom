#ifndef PROJECT_UI_GAME_OVER_H
#define PROJECT_UI_GAME_OVER_H

#include <stdbool.h>

#include "game_input.h"
#include "game_state.h"
#include "sprite.h"

typedef enum {
  GAME_OVER_SEL_RESTART = 0,
  GAME_OVER_SEL_MENU,
  GAME_OVER_SEL_EXIT
} GameOverSelection;

typedef struct {
  GameOverSelection selection;
  int               winner;
  Sprite            button;
  Sprite            button_selected;
  Sprite            title;
  Sprite            player1_wins_text;
  Sprite            player2_wins_text;
  Sprite            retry_text;
  Sprite            back_text;
  Sprite            exit_text;
  bool              assets_loaded;
} GameOverState;

void game_over_state_init(GameOverState *state, int winner);
int  game_over_state_load_assets(GameOverState *state);
void game_over_state_destroy_assets(GameOverState *state);
void game_over_state_update(GameOverState *state, const GameInputActions *actions, GameState *next);
int  game_over_state_render(const GameOverState *state);

#endif
