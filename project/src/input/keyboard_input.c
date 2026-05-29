#include "keyboard_input.h"

#include <stddef.h>

#define KEYBOARD_BREAK_BIT 0x80
#define KEYBOARD_MAKE_MASK 0x7F

#define W_MAKECODE 0x11
#define E_MAKECODE 0x12
#define SPACE_MAKECODE 0x39
#define ESC_BREAKCODE 0x81

void keyboard_input_init(KeyboardInput *input) {
  if (input == NULL) {
    return;
  }

  input->move_forward = false;
  input->shoot = false;
  input->action = false;
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

  // Bit 7 marks break codes.
  pressed = (scancode & KEYBOARD_BREAK_BIT) == 0;
  makecode = scancode & KEYBOARD_MAKE_MASK;

  switch (makecode) {
    case W_MAKECODE:
      input->move_forward = pressed;
      break;
    case SPACE_MAKECODE:
      input->shoot = pressed;
      break;
    case E_MAKECODE:
      input->action = pressed;
      break;
    default:
      break;
  }
}
