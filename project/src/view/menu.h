/**
 * @file menu.h
 * @brief Main menu and difficulty menu state.
 *
 * The menu has a root screen and a difficulty picker. Keyboard and mouse
 * navigation update the selected item, then menu_state_update() chooses the
 * next game state when the player confirms.
 */
#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "game_input.h"
#include "game_state.h"
#include "mouse_input.h"
#include "sprite.h"

/**
 * @brief Visible menu screen.
 */
typedef enum {
  MENU_SCREEN_MAIN = 0,  /**< Root menu with Start, Difficulty, Log, Exit. */
  MENU_SCREEN_DIFFICULTY /**< Difficulty picker with Easy, Medium, Hard, Back. */
} MenuScreen;

/**
 * @brief Selection on the root menu.
 */
typedef enum {
  MENU_SEL_START = 0, /**< Start button. */
  MENU_SEL_DIFFICULTY, /**< Difficulty button. */
  MENU_SEL_LOG,        /**< Log button. */
  MENU_SEL_EXIT        /**< Exit button. */
} MenuSelection;

/**
 * @brief Selection on the difficulty menu.
 */
typedef enum {
  MENU_DIFF_SEL_EASY = 0, /**< Easy difficulty. */
  MENU_DIFF_SEL_MEDIUM,   /**< Medium difficulty. */
  MENU_DIFF_SEL_HARD,     /**< Hard difficulty. */
  MENU_DIFF_SEL_BACK      /**< Return to main menu. */
} MenuDiffSelection;

/**
 * @brief Runtime state and sprites for the menu.
 */
typedef struct {
  MenuScreen        screen;            /**< Currently visible sub-screen. */
  MenuSelection     selection;         /**< Highlighted item on the main screen. */
  MenuDiffSelection diff_selection;    /**< Highlighted item on the difficulty screen. */
  ArenaDifficulty   selected_difficulty; /**< Difficulty chosen by the player. */
  Sprite            button;            /**< Unselected button background sprite. */
  Sprite            button_selected;   /**< Selected/hovered button background sprite. */
  Sprite            start_text;        /**< "Start" label sprite. */
  Sprite            difficulty_text;   /**< "Difficulty" label sprite. */
  Sprite            log_text;          /**< "Log" label sprite. */
  Sprite            exit_text;         /**< "Exit" label sprite. */
  Sprite            select_diff_title; /**< "Select Difficulty" title sprite. */
  Sprite            easy_text;         /**< "Easy" label sprite. */
  Sprite            medium_text;       /**< "Medium" label sprite. */
  Sprite            hard_text;         /**< "Hard" label sprite. */
  Sprite            back_text;         /**< "Back" label sprite. */
  Sprite            cursor;            /**< Mouse cursor sprite. */
  int16_t           cursor_x;          /**< Current cursor x position in pixels. */
  int16_t           cursor_y;          /**< Current cursor y position in pixels. */
  bool              prev_lb;           /**< Left button state from the previous frame, for edge detection. */
} MenuState;

/**
 * @brief Initializes default selections, cursor and sprite handles.
 */
void menu_state_init(MenuState *menu);

/**
 * @brief Resets selection and cursor without reloading assets.
 */
void menu_state_reset(MenuState *menu);

/**
 * @brief Moves the cursor and clamps it to the screen.
 */
void menu_state_move_cursor(MenuState *menu, int16_t dx, int16_t dy);

/**
 * @brief Converts a left mouse click over a button into confirm.
 *
 * Hover state is also updated from the current cursor position.
 */
void menu_state_apply_mouse(MenuState *menu, const MouseInput *mouse, GameInputActions *actions);

/**
 * @brief Handles navigation and writes the requested next state.
 */
void menu_state_update(MenuState *menu, const GameInputActions *actions, GameState *next);

/**
 * @brief Draws the active menu screen.
 */
int menu_state_render(const MenuState *menu);

/**
 * @brief Loads all menu sprites.
 */
int menu_state_load_assets(MenuState *menu);

/**
 * @brief Frees all menu sprites.
 */
void menu_state_destroy_assets(MenuState *menu);

#endif
