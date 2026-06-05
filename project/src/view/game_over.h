/**
 * @file game_over.h
 * @brief Game over screen state and rendering.
 *
 * Shows the winner and lets the player retry or return to the main menu.
 */
#ifndef GAME_OVER_H
#define GAME_OVER_H

#include <stdbool.h>
#include <stdint.h>

#include "game_input.h"
#include "game_state.h"
#include "mouse_input.h"
#include "sprite.h"

/**
 * @brief Game over menu options.
 */
typedef enum {
  GAME_OVER_SEL_RESTART = 0, /**< Start a new match with the same settings. */
  GAME_OVER_SEL_MENU         /**< Return to the main menu. */
} GameOverSelection;

/**
 * @brief Runtime state and sprites for the game over screen.
 */
typedef struct {
  GameOverSelection selection;        /**< Currently highlighted option. */
  int               winner;           /**< Winning player number (1 or 2). */
  Sprite            button;           /**< Unselected button background sprite. */
  Sprite            button_selected;  /**< Selected/hovered button background sprite. */
  Sprite            title;            /**< "Game Over" title sprite. */
  Sprite            player1_wins_text;/**< "Player 1 Wins" announcement sprite. */
  Sprite            player2_wins_text;/**< "Player 2 Wins" announcement sprite. */
  Sprite            retry_text;       /**< "Retry" label sprite. */
  Sprite            back_text;        /**< "Main Menu" label sprite. */
  Sprite            cursor;           /**< Mouse cursor sprite. */
  int16_t           cursor_x;         /**< Current cursor x position in pixels. */
  int16_t           cursor_y;         /**< Current cursor y position in pixels. */
  bool              prev_lb;          /**< Left button state from the previous frame. */
} GameOverState;

/**
 * @brief Initializes winner, selection, cursor and sprite handles.
 */
void game_over_state_init(GameOverState *state, int winner);

/**
 * @brief Loads all game-over screen sprites.
 */
int game_over_state_load_assets(GameOverState *state);

/**
 * @brief Frees all game-over screen sprites.
 */
void game_over_state_destroy_assets(GameOverState *state);

/**
 * @brief Moves the cursor and clamps it to the screen.
 */
void game_over_state_move_cursor(GameOverState *state, int16_t dx, int16_t dy);

/**
 * @brief Converts a left mouse click over an option into confirm.
 */
void game_over_state_apply_mouse(GameOverState *state, const MouseInput *mouse, GameInputActions *actions);

/**
 * @brief Handles game over navigation and selected action.
 */
void game_over_state_update(GameOverState *state, const GameInputActions *actions, GameState *next);

/**
 * @brief Draws the game over screen.
 */
int game_over_state_render(const GameOverState *state);

#endif
