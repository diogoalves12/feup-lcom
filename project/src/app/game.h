/**
 * @file game.h
 * @brief Entry point for the game.
 *
 * game.c implements the main state machine, device subscriptions and frame flow.
 * Timer interrupts manages updates and rendering.
 * Keyboard and mouse interrupts only update input state.
 *
 * Rendering uses double buffering. Each frame is drawn to the back buffer and shown with renderer_present().
 */
#ifndef GAME_H
#define GAME_H

/**
 * @brief Starts the game and returns when it exits.
 */
int game_main_loop(int argc, char *argv[]);

#endif
