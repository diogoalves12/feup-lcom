/**
 * @file keyboard_input.h
 * @brief Converts raw keyboard scancodes into key state.
 *
 * Movement and shooting stay true while the key is pressed. 
 * Menu and pause events are one frame flags and must be cleared after they are read.
 *
 * Keyboard layout:
 *  - Player 1: W to move forward, Space to shoot
 *  - Menus: arrow up and arrow down to navigate, Enter to confirm
 *  - System: Escape to go back or exit, P to pause the game
 */
#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Keyboard state.
 */
typedef struct {
  bool move_forward;  /**< True while the forward key is pressed. */
  bool shoot;         /**< True while the shoot key is pressed. */
  bool nav_up;        /**< True while the up key is pressed. */
  bool nav_down;      /**< True while the down key is pressed. */
  bool escape;        /**< Set if escape key is pressed. */
  bool confirm;       /**< Set if confirm key is pressed. */
  bool pause_toggle;  /**< Set if pause key is pressed. */
  bool _extended;     /**< Tracks the 0xE0 extended key prefix. */
} KeyboardInput;

/**
 * @brief Clears the keyboard state.
 */
void keyboard_input_init(KeyboardInput *input);

/**
 * @brief Applies one raw scancode to the keyboard state.
 *
 * Handles make codes, break codes and the 0xE0 prefix used by arrow keys.
 */
void keyboard_input_update(KeyboardInput *input, uint8_t scancode);

/**
 * @brief Clears keyboard actions that should last only one frame.
 */
void keyboard_input_clear_frame_actions(KeyboardInput *input);

#endif
