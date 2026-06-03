#ifndef PROJECT_CONTROLLER_GAME_INPUT_H
#define PROJECT_CONTROLLER_GAME_INPUT_H

#include <stdbool.h>

#include "keyboard_input.h"
#include "mouse_input.h"

typedef struct {
  bool move_forward;
  bool shoot;
  bool action;
} PlayerInputActions;

typedef struct {
  PlayerInputActions player1;
  PlayerInputActions player2;
  bool nav_up;
  bool nav_down;
  bool confirm;
  bool back;
  bool pause_requested;
} GameInputActions;

void game_input_actions_init(GameInputActions *actions);
void game_input_actions_from_keyboard(GameInputActions *actions, const KeyboardInput *keyboard);
void game_input_actions_apply_mouse(GameInputActions *actions, const MouseInput *mouse);

#endif
