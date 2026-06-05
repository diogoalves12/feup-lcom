/**
 * @file mouse_input.h
 * @brief Mouse button state used by Player 2 and menus.
 *
 * Left and right buttons are mapped to gameplay actions. Mouse movement is
 * handled in game.c because each screen uses the cursor differently.
 *
 * Button mapping:
 *  - Left button (lb)  = move forward
 *  - Right button (rb) = shoot
 */
#ifndef MOUSE_INPUT_H
#define MOUSE_INPUT_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Current mouse button state.
 */
typedef struct {
  bool move_forward; /**< True while the left mouse button is held. */
  bool shoot;        /**< True while the right mouse button is held. */
} MouseInput;

/**
 * @brief Clears the mouse button state.
 */
void mouse_input_init(MouseInput *input);

/**
 * @brief Updates button flags from a parsed mouse packet.
 */
void mouse_input_set(MouseInput *input, bool lb, bool rb);

#endif
