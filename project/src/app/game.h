/**
 * @file game.h
 * @brief Entry point for the game.
 *
 * game.c owns the main state machine, device subscriptions and frame flow.
 * Timer interrupts drive updates and rendering. Keyboard and mouse interrupts
 * only update input state.
 *
 * Rendering is double buffered. Each frame is drawn to the back buffer and
 * shown with renderer_present().
 */
#ifndef GAME_H
#define GAME_H

/**
 * @brief Starts the game and returns when it exits.
 */
int game_main_loop(int argc, char *argv[]);

#endif
