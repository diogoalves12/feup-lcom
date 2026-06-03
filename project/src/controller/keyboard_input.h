#ifndef PROJECT_CONTROLLER_KEYBOARD_INPUT_H
#define PROJECT_CONTROLLER_KEYBOARD_INPUT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool move_forward;
  bool shoot;
  bool action;
  bool nav_up;
  bool nav_down;
  bool escape;
  bool confirm;
  bool pause_toggle;
  bool _extended;
} KeyboardInput;

void keyboard_input_init(KeyboardInput *input);
void keyboard_input_update(KeyboardInput *input, uint8_t scancode);
void keyboard_input_clear_oneshots(KeyboardInput *input);

#endif
