/**
 * @file game_state.h
 * @brief States used by the main game loop.
 *
 * Each value represents one screen or one phase of the game.
 */
#ifndef GAME_STATE_H
#define GAME_STATE_H

/**
 * @brief Current state/screen.
 */
typedef enum {
  GAME_STATE_MENU = 0,  /**< Main menu is displayed. */
  GAME_STATE_PLAYING,   /**< Active 1v1 match in progress. */
  GAME_STATE_PAUSED,    /**< Match is paused: pause menu shown. */
  GAME_STATE_GAME_OVER, /**< A player has died: game over screen shown. */
  GAME_STATE_LOG,       /**< Match history log screen is displayed. */
  GAME_STATE_EXIT       /**< Exit requested: game loop will terminate. */
} GameState;

#endif
