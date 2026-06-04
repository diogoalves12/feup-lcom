#ifndef PROJECT_CONTROLLER_MOUSE_INPUT_H
#define PROJECT_CONTROLLER_MOUSE_INPUT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool move_forward;
  bool shoot;
  bool action;
} MouseInput;

void mouse_input_init(MouseInput *input);
void mouse_input_set(MouseInput *input, bool lb, bool rb, bool mb);

#endif
