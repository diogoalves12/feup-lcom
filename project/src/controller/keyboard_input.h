#ifndef PROJECT_INPUT_KEYBOARD_INPUT_H
#define PROJECT_INPUT_KEYBOARD_INPUT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  /* Held keys: true while the key is physically pressed. */
  bool move_forward;
  bool shoot;
  bool action;

  /* One-shot events: set on key press, cleared by keyboard_input_clear_oneshots()
   * at the end of each game tick so they are true for exactly one tick. */
  bool nav_up;
  bool nav_down;
  bool escape;
  bool confirm;
  bool pause_toggle;
  bool debug_damage_p1;
  bool debug_damage_p2;

  /* Internal: tracks the 0xE0 extended-scancode prefix. */
  bool _extended;
} KeyboardInput;

void keyboard_input_init(KeyboardInput *input);

/* Called per raw scancode from the KBC interrupt handler. */
void keyboard_input_update(KeyboardInput *input, uint8_t scancode);

/* Called once per game tick, after actions have been read, to reset one-shots. */
void keyboard_input_clear_oneshots(KeyboardInput *input);

#endif
