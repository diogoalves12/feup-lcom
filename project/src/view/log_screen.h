/**
 * @file log_screen.h
 * @brief Match log screen.
 *
 * Displays the matches stored in MatchLog and provides a Back button to
 * return to the menu.
 */
#ifndef LOG_SCREEN_H
#define LOG_SCREEN_H

#include <stdbool.h>
#include <stdint.h>

#include "game_input.h"
#include "game_state.h"
#include "match_log.h"
#include "mouse_input.h"
#include "sprite.h"

/**
 * @brief Runtime state and sprites for the log screen.
 */
typedef struct {
  Sprite  button;          /**< Unselected button background sprite. */
  Sprite  button_selected; /**< Selected/hovered button background sprite. */
  Sprite  back_text;       /**< "Back" label sprite. */
  Sprite  cursor;          /**< Mouse cursor sprite. */
  int16_t cursor_x;        /**< Current cursor x position in pixels. */
  int16_t cursor_y;        /**< Current cursor y position in pixels. */
  bool    prev_lb;         /**< Left button state from the previous frame. */
  bool    back_hover;      /**< True when the cursor is over the Back button. */
} LogScreenState;

/**
 * @brief Initializes cursor, hover state and sprite handles.
 */
void log_screen_init(LogScreenState *state);

/**
 * @brief Loads all log screen sprites.
 */
int log_screen_load_assets(LogScreenState *state);

/**
 * @brief Frees all log screen sprites.
 */
void log_screen_destroy_assets(LogScreenState *state);

/**
 * @brief Resets cursor position and Back button hover.
 */
void log_screen_reset(LogScreenState *state);

/**
 * @brief Moves the cursor and clamps it to the screen.
 */
void log_screen_move_cursor(LogScreenState *state, int16_t dx, int16_t dy);

/**
 * @brief Converts a left click on Back into confirm.
 */
void log_screen_apply_mouse(LogScreenState *state, const MouseInput *mouse, GameInputActions *actions);

/**
 * @brief Returns to the main menu on confirm or back.
 */
void log_screen_update(LogScreenState *state, const GameInputActions *actions, GameState *next);

/**
 * @brief Draws all valid match entries and the Back button.
 */
int log_screen_render(const LogScreenState *state, const MatchLog *log);

#endif
