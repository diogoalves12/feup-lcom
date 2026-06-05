/**
 * @file mouse_input.h
 * @brief Mouse button state used by Player 2 and menus.
 *
 * Left and right buttons are mapped to gameplay actions.
 * Mouse movement is handled in game.c because each screen implements it differently.
 *
 * Button mapping:
 *  Left button (lb) = move forward
 *  Right button (rb) = shoot
 */
#ifndef MOUSE_INPUT_H
#define MOUSE_INPUT_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Mouse button state.
 */
typedef struct {
  bool move_forward; /**< True while the left mouse button is pressed. */
  bool shoot;        /**< True while the right mouse button is pressed. */
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
