/**
 * @file game_input.h
 * @brief Combines keyboard and mouse input into game actions.
 *
 * Game code reads this struct instead of checking raw scancodes or mouse
 * packets directly. Keyboard controls Player 1 and menus. Mouse controls
 * Player 2 and the menu cursor.
 */
#ifndef GAME_INPUT_H
#define GAME_INPUT_H

#include <stdbool.h>

#include "keyboard_input.h"
#include "mouse_input.h"

/**
 * @brief Actions available to each player during gameplay.
 */
typedef struct {
  bool move_forward; /**< Player should move forward this frame. */
  bool shoot;        /**< Player should attempt to fire this frame. */
} PlayerInputActions;

/**
 * @brief Gameplay and menu actions for the current frame.
 */
typedef struct {
  PlayerInputActions player1;        /**< Actions for Player 1 (keyboard). */
  PlayerInputActions player2;        /**< Actions for Player 2 (mouse). */
  bool               nav_up;         /**< Menu: navigate to the previous item. */
  bool               nav_down;       /**< Menu: navigate to the next item. */
  bool               confirm;        /**< Menu: confirm the current selection. */
  bool               back;           /**< Menu: go back to the previous screen. */
  bool               pause_requested;/**< In-game: pause toggle was pressed. */
} GameInputActions;

/**
 * @brief Clears every action flag.
 */
void game_input_actions_init(GameInputActions *actions);

/**
 * @brief Reads Player 1 and menu actions from the keyboard state.
 */
void game_input_actions_from_keyboard(GameInputActions *actions, const KeyboardInput *keyboard);

/**
 * @brief Adds Player 2 actions from the mouse buttons.
 *
 * Mouse movement is handled separately by the active screen.
 */
void game_input_actions_apply_mouse(GameInputActions *actions, const MouseInput *mouse);

#endif
