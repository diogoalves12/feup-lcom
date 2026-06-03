#include "player_view.h"

#include <stddef.h>
#include <math.h>

#include "renderer.h"

#define PLAYER_DIRECTION_INDICATOR_SIZE  4
#define PLAYER_DIRECTION_INDICATOR_COLOR 0xFFFFFF

int player_view_draw(const Player *player) {
  if (player == NULL) return 1;

  if (!player->alive) return 0;

  int x = player->position.x - player->width / 2;
  int y = player->position.y - player->height / 2;

  if (x < 0 || y < 0) return 1;

  if (renderer_draw_rectangle((uint16_t) x, (uint16_t) y, player->width, player->height, player->color) != 0) {
    return 1;
  }

  int front_x = player->position.x + (int) ((player->width / 2) * cosf(player->angle));
  int front_y = player->position.y + (int) ((player->height / 2) * sinf(player->angle));
  int indicator_x = front_x - PLAYER_DIRECTION_INDICATOR_SIZE / 2;
  int indicator_y = front_y - PLAYER_DIRECTION_INDICATOR_SIZE / 2;

  if (indicator_x < 0 || indicator_y < 0) return 0;

  if (renderer_draw_rectangle((uint16_t) indicator_x, (uint16_t) indicator_y, PLAYER_DIRECTION_INDICATOR_SIZE, PLAYER_DIRECTION_INDICATOR_SIZE, PLAYER_DIRECTION_INDICATOR_COLOR) != 0) {
    return 1;
  }

  return 0;
}

void player_view_draw_health_bar(const Player *player, int screen_x, int screen_y) {
  if (player == NULL || !player->alive || player->health <= 0) return;

  int hp_width  = 15;
  int bar_width = player->health * hp_width;
  int bar_height = 8;

  renderer_draw_rectangle(screen_x, screen_y, bar_width, bar_height, player->color);
}
