#ifndef PROJECT_MODEL_PLAYER_H
#define PROJECT_MODEL_PLAYER_H

#include <stdbool.h>
#include <stdint.h>

#include "types.h"

#define PLAYER_DEFAULT_HEALTH 3
#define PLAYER_DEFAULT_WIDTH 22
#define PLAYER_DEFAULT_HEIGHT 22
#define PLAYER1_INITIAL_ANGLE 0.0f
#define PLAYER2_INITIAL_ANGLE 3.1415926f
#define PLAYER1_COLOR 0x00AAFF
#define PLAYER2_COLOR 0xFF4040
#define PLAYER_FULL_ROTATION 6.2831852f
#define PLAYER_ROTATION_STEP 0.10f
#define PLAYER_MOVE_SPEED 3.0f

typedef struct {
  Position position;
  float angle;
  int health;
  bool alive;
  uint16_t width;
  uint16_t height;
  uint32_t color;
} Player;

void player_init(Player *player, Position spawn, float angle, uint32_t color);
bool player_is_alive(const Player *player);
void player_kill(Player *player);
void player_damage(Player *player, int damage);
void player_set_position(Player *player, Position position);
Position player_get_position(const Player *player);
void player_rotate(Player *player, float delta_angle);

Position player_get_forward_position(const Player *player, float distance);
void player_move_forward(Player *player, float distance);

#endif
