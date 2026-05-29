#include "game_input.h"

#include <stddef.h>

static void player_input_actions_clear(PlayerInputActions *actions) {
  if (actions == NULL) {
    return;
  }

  actions->move_forward = false;
  actions->shoot = false;
  actions->action = false;
}

void game_input_actions_init(GameInputActions *actions) {
  if (actions == NULL) {
    return;
  }

  player_input_actions_clear(&actions->player1);
  player_input_actions_clear(&actions->player2);
  actions->exit_requested = false;
}

void game_input_actions_from_keyboard(GameInputActions *actions, const KeyboardInput *keyboard) {
  if (actions == NULL || keyboard == NULL) {
    return;
  }

  // Keyboard controls player 1.
  actions->player1.move_forward = keyboard->move_forward;
  actions->player1.shoot = keyboard->shoot;
  actions->player1.action = keyboard->action;
  actions->exit_requested = keyboard->exit_requested;
}
