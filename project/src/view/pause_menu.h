/**
 * @file pause_menu.h
 * @brief Pause menu state and rendering.
 *
 * The pause menu is drawn over the current match and can resume, restart or
 * return to the main menu.
 */
#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include <stdbool.h>
#include <stdint.h>

#include "game_input.h"
#include "game_state.h"
#include "mouse_input.h"
#include "sprite.h"

/**
 * @brief Pause menu options.
 */
typedef enum {
  PAUSE_SEL_CONTINUE  = 0, /**< Resume the current match. */
  PAUSE_SEL_RESTART   = 1, /**< Restart the match from the beginning. */
  PAUSE_SEL_MAIN_MENU = 2  /**< Return to the main menu. */
} PauseSelection;

/**
 * @brief Runtime state and sprites for the pause menu.
 */
typedef struct {
  PauseSelection selection;       /**< Currently highlighted option. */
  Sprite         button;          /**< Unselected button background sprite. */
  Sprite         button_selected; /**< Selected/hovered button background sprite. */
  Sprite         title;           /**< "Pause" title sprite. */
  Sprite         resume_text;     /**< "Resume" label sprite. */
  Sprite         retry_text;      /**< "Retry" label sprite. */
  Sprite         back_text;       /**< "Main Menu" label sprite. */
  Sprite         cursor;          /**< Mouse cursor sprite. */
  int16_t        cursor_x;        /**< Current cursor x position in pixels. */
  int16_t        cursor_y;        /**< Current cursor y position in pixels. */
  bool           prev_lb;         /**< Left button state from the previous frame. */
} PauseMenuState;

/**
 * @brief Initializes default selection, cursor and sprite handles.
 */
void pause_menu_state_init(PauseMenuState *menu);

/**
 * @brief Loads all pause menu sprites.
 */
int pause_menu_state_load_assets(PauseMenuState *menu);

/**
 * @brief Frees all pause menu sprites.
 */
void pause_menu_state_destroy_assets(PauseMenuState *menu);

/**
 * @brief Resets the cursor when entering pause.
 */
void pause_menu_state_reset_cursor(PauseMenuState *menu);

/**
 * @brief Moves the cursor and clamps it to the screen.
 */
void pause_menu_state_move_cursor(PauseMenuState *menu, int16_t dx, int16_t dy);

/**
 * @brief Converts a left mouse click over an option into confirm.
 */
void pause_menu_state_apply_mouse(PauseMenuState *menu, const MouseInput *mouse, GameInputActions *actions);

/**
 * @brief Handles pause menu navigation and selected action.
 */
void pause_menu_state_update(PauseMenuState *menu, const GameInputActions *actions,
                             GameState *next);

/**
 * @brief Draws the pause overlay on top of the current back buffer contents.
 */
int pause_menu_state_render(const PauseMenuState *menu);

#endif
