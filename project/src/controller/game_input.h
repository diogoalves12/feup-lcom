/**
 * @file game_input.h
 * @brief Combines keyboard and mouse input into game actions.
 *
 * Game code reads this struct instead of checking raw scancodes or mouse packets directly. 
 * Keyboard controls Player 1 and menus. 
 * Mouse controls Player 2 and the menu cursor.
 */
#ifndef GAME_INPUT_H
#define GAME_INPUT_H

#include <stdbool.h>

#include "keyboard_input.h"
#include "mouse_input.h"

/**
 * @brief Actions available to each player during the game.
 */
typedef struct {
  bool move_forward;
  bool shoot;
} PlayerInputActions;

/**
 * @brief Gameplay and menu actions for the current frame.
 */
typedef struct {
  PlayerInputActions player1;        /**< Player 1 uses keyboard. */
  PlayerInputActions player2;        /**< Player 2 uses mouse. */
  bool nav_up;         
  bool nav_down;       
  bool confirm;        
  bool back;           
  bool pause_requested;
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
 * @brief Reads Player 2 actions from the mouse buttons.
 *
 * Cursor movement is handled separately by the active screen.
 */
void game_input_actions_apply_mouse(GameInputActions *actions, const MouseInput *mouse);

#endif
