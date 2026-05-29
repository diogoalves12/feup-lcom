#include "keyboard_input.h"

#include <stddef.h>

#define KEYBOARD_BREAK_BIT 0x80
#define KEYBOARD_MAKE_MASK 0x7F

#define W_MAKECODE 0x11
#define A_MAKECODE 0x1E
#define S_MAKECODE 0x1F
#define D_MAKECODE 0x20
#define SPACE_MAKECODE 0x39
#define ESC_BREAKCODE 0x81

void keyboard_input_init(KeyboardInput *input) {
  if (input == NULL) {
    return;
  }

  input->up = false;
  input->down = false;
  input->left = false;
  input->right = false;
  input->shoot = false;
  input->exit_requested = false;
}

void keyboard_input_update(KeyboardInput *input, uint8_t scancode) {
  bool pressed;
  uint8_t makecode;

  if (input == NULL) {
    return;
  }

  if (scancode == ESC_BREAKCODE) {
    input->exit_requested = true;
    return;
  }

  pressed = (scancode & KEYBOARD_BREAK_BIT) == 0;
  makecode = scancode & KEYBOARD_MAKE_MASK;

  switch (makecode) {
    case W_MAKECODE:
      input->up = pressed;
      break;
    case A_MAKECODE:
      input->left = pressed;
      break;
    case S_MAKECODE:
      input->down = pressed;
      break;
    case D_MAKECODE:
      input->right = pressed;
      break;
    case SPACE_MAKECODE:
      input->shoot = pressed;
      break;
    default:
      break;
  }
}
