#include "keyboard_input.h"

#include <stddef.h>

#define KEYBOARD_BREAK_BIT  0x80
#define KEYBOARD_MAKE_MASK  0x7F

/* Scancodes (Set 1). */
#define ESC_MAKECODE   0x01
#define W_MAKECODE     0x11
#define E_MAKECODE     0x12
#define P_MAKECODE     0x19
#define ENTER_MAKECODE 0x1C
#define SPACE_MAKECODE 0x39

/* Extended prefix byte sent before arrows and other extended keys. */
#define KEYBOARD_EXTENDED 0xE0

/* Extended makecodes (sent after 0xE0). */
#define ARROW_UP_EXT   0x48
#define ARROW_DOWN_EXT 0x50

void keyboard_input_init(KeyboardInput *input) {
  if (input == NULL) return;
  input->move_forward  = false;
  input->shoot         = false;
  input->action        = false;
  input->nav_up        = false;
  input->nav_down      = false;
  input->escape        = false;
  input->confirm       = false;
  input->pause_toggle  = false;
  input->_extended     = false;
}

void keyboard_input_clear_oneshots(KeyboardInput *input) {
  if (input == NULL) return;
  input->nav_up       = false;
  input->nav_down     = false;
  input->escape       = false;
  input->confirm      = false;
  input->pause_toggle = false;
}

void keyboard_input_update(KeyboardInput *input, uint8_t scancode) {
  if (input == NULL) return;

  /* First byte of an extended key sequence, record and wait for the second. */
  if (scancode == KEYBOARD_EXTENDED) {
    input->_extended = true;
    return;
  }

  if (input->_extended) {
    input->_extended = false;
    bool pressed  = (scancode & KEYBOARD_BREAK_BIT) == 0;
    uint8_t make  = scancode & KEYBOARD_MAKE_MASK;
    switch (make) {
      case ARROW_UP_EXT:   if (pressed) input->nav_up   = true; break;
      case ARROW_DOWN_EXT: if (pressed) input->nav_down = true; break;
      default: break;
    }
    return;
  }

  /* ESC is one shot on press. */
  if (scancode == ESC_MAKECODE) {
    input->escape = true;
    return;
  }

  bool pressed = (scancode & KEYBOARD_BREAK_BIT) == 0;
  uint8_t make = scancode & KEYBOARD_MAKE_MASK;

  switch (make) {
    case W_MAKECODE:     input->move_forward = pressed;            break;
    case SPACE_MAKECODE: input->shoot        = pressed;            break;
    case E_MAKECODE:     input->action       = pressed;            break;
    case ENTER_MAKECODE: if (pressed) input->confirm      = true;  break;
    case P_MAKECODE:     if (pressed) input->pause_toggle = true;  break;
    default: break;
  }
}
