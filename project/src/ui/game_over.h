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
  int winner; /* 1 or 2 */
} GameOverState;

void game_over_state_init(GameOverState *state, int winner);

/* Called once per game tick while in GAME_STATE_GAME_OVER. */
void game_over_state_update(GameOverState *state, const GameInputActions *actions, GameState *next);

/* Draws the game-over screen. Returns 0 on success. */
int game_over_state_render(const GameOverState *state);

#endif
