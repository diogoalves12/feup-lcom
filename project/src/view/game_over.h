#ifndef PROJECT_UI_GAME_OVER_H
#define PROJECT_UI_GAME_OVER_H

#include "game_input.h"
#include "game_state.h"

typedef enum {
  GAME_OVER_SEL_RESTART = 0,
  GAME_OVER_SEL_MENU,
  GAME_OVER_SEL_EXIT
} GameOverSelection;

typedef struct {
  GameOverSelection selection;
  int winner;
} GameOverState;

void game_over_state_init(GameOverState *state, int winner);
void game_over_state_update(GameOverState *state, const GameInputActions *actions, GameState *next);
int game_over_state_render(const GameOverState *state);

#endif
