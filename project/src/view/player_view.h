/**
 * @file player_view.h
 * @brief Player sprites and health bar rendering.
 *
 * Player sprites are rotated to match their angle. Health bars are drawn
 * with simple rectangles in the HUD.
 */
#ifndef PLAYER_VIEW_H
#define PLAYER_VIEW_H

#include "player.h"
#include "sprite.h"

/**
 * @brief Sprites used by player rendering.
 */
typedef struct {
  Sprite player1; /**< Player 1 sprite (blue soldier). */
  Sprite player2; /**< Player 2 sprite (red soldier). */
  Sprite bullet;  /**< Bullet sprite used by draw_shot_effect in game.c. */
} PlayerViewAssets;

/**
 * @brief Loads all player and bullet sprites.
 */
int player_view_load_assets(PlayerViewAssets *assets);

/**
 * @brief Frees all player and bullet sprites.
 */
void player_view_destroy_assets(PlayerViewAssets *assets);

/**
 * @brief Draws one player centered at its position.
 */
int player_view_draw(const Player *player, const PlayerViewAssets *assets, int player_id);

/**
 * @brief Draws the player's health bar at the given HUD position.
 *
 * Renders filled rectangles: one per hit point remaining (green) and
 * one per lost hit point (dark).
 */
void player_view_draw_health_bar(const Player *player, int screen_x, int screen_y);

/**
 * @brief Returns the total pixel width of the health bar widget.
 *
 * Used by the caller to right-align Player 2's health bar.
 */
int player_view_health_bar_width(void);

#endif
