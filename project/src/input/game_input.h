#ifndef PROJECT_INPUT_GAME_INPUT_H
#define PROJECT_INPUT_GAME_INPUT_H

#include <stdbool.h>

#include "keyboard_input.h"

typedef struct {
  bool move_forward;
  bool shoot;
  bool action;
} PlayerInputActions;

typedef struct {
  PlayerInputActions player1;
  PlayerInputActions player2;
  bool exit_requested;
} GameInputActions;

void game_input_actions_init(GameInputActions *actions);
void game_input_actions_from_keyboard(GameInputActions *actions, const KeyboardInput *keyboard);

#endif /* PROJECT_INPUT_GAME_INPUT_H */
