#ifndef PROJECT_INPUT_MOUSE_INPUT_H
#define PROJECT_INPUT_MOUSE_INPUT_H

#include <stdbool.h>

typedef struct {
  bool move_forward; /* left button  */
  bool shoot;        /* right button */
  bool action;       /* middle button */
} MouseInput;

void mouse_input_init(MouseInput *input);
void mouse_input_set(MouseInput *input, bool lb, bool rb, bool mb);

#endif
