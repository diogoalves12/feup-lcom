#include "mouse_input.h"

#include <stddef.h>

void mouse_input_init(MouseInput *input) {
  if (input == NULL) return;
  input->move_forward = false;
  input->shoot        = false;
  input->action       = false;
}

void mouse_input_set(MouseInput *input, bool lb, bool rb, bool mb) {
  if (input == NULL) return;
  input->move_forward = lb;
  input->shoot        = rb;
  input->action       = mb;
}
