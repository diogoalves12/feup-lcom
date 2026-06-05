/**
 * @file keyboard_input.h
 * @brief Converts raw keyboard scancodes into key state.
 *
 * Movement and shooting stay true while the key is held. Menu and pause
 * events are one frame flags and must be cleared after they are read.
 *
 * Keyboard layout:
 *  - Player 1: W = move forward, Space = shoot
 *  - Menus: arrow up and arrow down = navigation, Enter = confirm
 *  - Shared: Escape = back or exit, P = pause
 */
#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Current logical keyboard state.
 */
typedef struct {
  bool move_forward;  /**< True while the forward movement key is held. */
  bool shoot;         /**< True while the shoot key is held. */
  bool nav_up;        /**< True while the menu-navigate-up key is held. */
  bool nav_down;      /**< True while the menu-navigate-down key is held. */
  bool escape;        /**< One-shot: set on Escape key press. */
  bool confirm;       /**< One-shot: set on confirm key press. */
  bool pause_toggle;  /**< One-shot: set on pause key press. */
  bool _extended;     /**< Internal: tracks the 0xE0 extended-key prefix. */
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
 * @brief Clears one frame events after the game reads them.
 */
void keyboard_input_clear_oneshots(KeyboardInput *input);

#endif
