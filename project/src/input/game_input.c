#include "game_input.h"

#include <stddef.h>

static void player_input_clear(PlayerInputActions *p) {
  if (p == NULL) return;
  p->move_forward = false;
  p->shoot        = false;
  p->action       = false;
}

void game_input_actions_init(GameInputActions *actions) {
  if (actions == NULL) return;
  player_input_clear(&actions->player1);
  player_input_clear(&actions->player2);
  actions->nav_up         = false;
  actions->nav_down       = false;
  actions->confirm        = false;
  actions->back           = false;
  actions->pause_requested = false;
}

void game_input_actions_from_keyboard(GameInputActions *actions, const KeyboardInput *keyboard) {
  if (actions == NULL || keyboard == NULL) return;

  /* Player 1 is controlled by the keyboard. */
  actions->player1.move_forward = keyboard->move_forward;
  actions->player1.shoot        = keyboard->shoot;
  actions->player1.action       = keyboard->action;

  /* Navigation and meta actions come from one-shot keyboard events. */
  actions->nav_up          = keyboard->nav_up;
  actions->nav_down        = keyboard->nav_down;
  actions->confirm         = keyboard->confirm;
  actions->back            = keyboard->escape;
  actions->pause_requested = keyboard->pause_toggle;
}
