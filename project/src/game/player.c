#include "player.h"

#include <math.h>
#include <stddef.h>

#include "renderer.h"

#define PLAYER_DIRECTION_INDICATOR_SIZE 4
#define PLAYER_DIRECTION_INDICATOR_COLOR 0xFFFFFF

void player_init(Player *player, Position spawn, float angle, uint32_t color) {
  if (player == NULL) {
    return;
  }

  player->position = spawn;
  player->angle = angle;
  player->health = PLAYER_DEFAULT_HEALTH;
  player->alive = true;
  player->width = PLAYER_DEFAULT_WIDTH;
  player->height = PLAYER_DEFAULT_HEIGHT;
  player->color = color;
}

bool player_is_alive(const Player *player) {
  if (player == NULL) {
    return false;
  }

  return player->alive;
}

void player_kill(Player *player) {
  if (player == NULL) {
    return;
  }

  player->health = 0;
  player->alive = false;
}

void player_damage(Player *player, int damage) {
  if (player == NULL || damage <= 0 || !player->alive) {
    return;
  }

  player->health -= damage;

  if (player->health <= 0) {
    player_kill(player);
  }
}

void player_set_position(Player *player, Position position) {
  if (player == NULL) {
    return;
  }

  player->position = position;
}

Position player_get_position(const Player *player) {
  if (player == NULL) {
    return (Position) {0, 0};
  }

  return player->position;
}

int player_draw(const Player *player) {
  if (player == NULL) {
    return 1;
  }

  if (!player->alive) {
    return 0;
  }

  int x = player->position.x - player->width / 2;
  int y = player->position.y - player->height / 2;

  if (x < 0 || y < 0) {
    return 1;
  }

  if (renderer_draw_rectangle((uint16_t) x,
                              (uint16_t) y,
                              player->width,
                              player->height,
                              player->color) != 0) {
    return 1;
  }

  int front_x = player->position.x + (int) ((player->width / 2) * cosf(player->angle));
  int front_y = player->position.y + (int) ((player->height / 2) * sinf(player->angle));
  int indicator_x = front_x - PLAYER_DIRECTION_INDICATOR_SIZE / 2;
  int indicator_y = front_y - PLAYER_DIRECTION_INDICATOR_SIZE / 2;

  if (indicator_x < 0 || indicator_y < 0) {
    return 0;
  }

  if (renderer_draw_rectangle((uint16_t) indicator_x,
                              (uint16_t) indicator_y,
                              PLAYER_DIRECTION_INDICATOR_SIZE,
                              PLAYER_DIRECTION_INDICATOR_SIZE,
                              PLAYER_DIRECTION_INDICATOR_COLOR) != 0) {
    return 1;
  }

  return 0;
}

void player_rotate(Player *player, float delta_angle) {
  if (player == NULL || !player->alive) {
    return;
  }

  player->angle += delta_angle;

  while (player->angle >= PLAYER_FULL_ROTATION) {
    player->angle -= PLAYER_FULL_ROTATION;
  }

  while (player->angle < 0.0f) {
    player->angle += PLAYER_FULL_ROTATION;
  }
}

Position player_get_forward_position(const Player *player, float distance) {
  if (player == NULL) {
    return (Position) {0, 0};
  }

  if (!player->alive || distance <= 0.0f) {
    return player->position;
  }

  int dx = (int) roundf(cosf(player->angle) * distance);
  int dy = (int) roundf(sinf(player->angle) * distance);

  return (Position) {player->position.x + dx, player->position.y + dy};
}

void player_move_forward(Player *player, float distance) {
  if (player == NULL || !player->alive || distance <= 0.0f) {
    return;
  }

  player->position = player_get_forward_position(player, distance);
}
