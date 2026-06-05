/**
 * @file config.h
 * @brief Global compile-time configuration.
 *
 * Keeps screen, renderer and gameplay tuning values in one place.
 */
#ifndef CONFIG_H
#define CONFIG_H

#define GAME_FREQUENCY     60         /**< Timer interrupt frequency in Hz. */
#define PROJECT_VIDEO_MODE 0x115      /**< VBE video mode: 800x600, 24-bit color. */
#define PROJECT_BG_COLOR   0x101010   /**< Background clear color (dark gray). */

// The renderer always uses double buffering.
// This macro is kept only as documentation/configuration information.
#define DOUBLE_BUFFER      1

#define SCREEN_WIDTH       800        /**< Framebuffer width in pixels. */
#define SCREEN_HEIGHT      600        /**< Framebuffer height in pixels. */

#define TILE_SIZE          20         /**< Arena tile side length in pixels. */
#define ARENA_COLS         40         /**< Number of arena tile columns. */
#define ARENA_ROWS         30         /**< Number of arena tile rows. */
#define BREAKABLE_WALL_HP  3          /**< Initial hit points for a breakable wall tile. */

#define PLAYER_DEFAULT_HEALTH  3          /**< Starting hit points per player. */
#define PLAYER_DEFAULT_WIDTH   22         /**< Player hitbox width in pixels. */
#define PLAYER_DEFAULT_HEIGHT  22         /**< Player hitbox height in pixels. */
#define PLAYER1_INITIAL_ANGLE  0.0f       /**< Player 1 facing angle at spawn (radians). */
#define PLAYER2_INITIAL_ANGLE  3.1415926f /**< Player 2 facing angle at spawn (radians, facing left). */
#define PLAYER1_COLOR          0x00AAFF   /**< Player 1 accent color (blue). */
#define PLAYER2_COLOR          0xFF4040   /**< Player 2 accent color (red). */
#define PLAYER_ROTATION_STEP   0.10f      /**< Angle increment per frame when not moving (radians). */
#define PLAYER_MOVE_SPEED      3.0f       /**< Forward movement distance per frame (pixels). */

#define COMBAT_DAMAGE              1   /**< Hit points deducted per successful shot. */
#define COMBAT_RAY_STEP            4   /**< Pixels advanced per ray-march iteration. */
#define COMBAT_AIM_TOLERANCE       14  /**< Extra pixels added to target hitbox for hit detection. */
#define COMBAT_MAX_DISTANCE        900 /**< Maximum ray travel distance in pixels. */
#define COMBAT_SHOT_COOLDOWN_TICKS 20  /**< Minimum frames between consecutive shots per player. */
#define COMBAT_SHOT_EFFECT_FRAMES  8   /**< Number of frames the bullet visual remains active. */

#define TELEPORT_COOLDOWN_FRAMES   30  /**< Minimum gameplay frames between teleports per player. */

#define PAUSE_BAR_COLOR    0xFFFF00   /**< Color of the pause indicator bar. */
#define PAUSE_BAR_HEIGHT   4          /**< Height in pixels of the pause indicator bar. */

#endif
