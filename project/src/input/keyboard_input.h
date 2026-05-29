#ifndef PROJECT_INPUT_KEYBOARD_INPUT_H
#define PROJECT_INPUT_KEYBOARD_INPUT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool up;
  bool down;
  bool left;
  bool right;
  bool shoot;
  bool exit_requested;
} KeyboardInput;

void keyboard_input_init(KeyboardInput *input);
void keyboard_input_update(KeyboardInput *input, uint8_t scancode);

#endif /* PROJECT_INPUT_KEYBOARD_INPUT_H */
