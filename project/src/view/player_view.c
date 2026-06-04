#include "player_view.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>

#include "renderer.h"

#include "xpm/player/player1/player_gun.xpm"
#include "xpm/player/player2/soldier1_gun.xpm"
#include "xpm/bullets/bullet_small.xpm"


#define PLAYER_SPRITE_ANGLE_OFFSET 0.0f

#define PLAYER_INDICATOR_RADIUS  10
#define PLAYER_INDICATOR_SIZE     4
#define PLAYER_INDICATOR_COLOR 0xFFFFFF

int player_view_load_assets(PlayerViewAssets *assets) {
  if (assets == NULL) return 1;
  sprite_init(&assets->p1);
  sprite_init(&assets->p2);
  sprite_init(&assets->bullet);
  assets->loaded = false;
  if (sprite_load(&assets->p1, player_gun_xpm)   != 0) { player_view_destroy_assets(assets); return 1; }
  if (sprite_load(&assets->p2, soldier1_gun_xpm) != 0) { player_view_destroy_assets(assets); return 1; }
  if (sprite_load(&assets->bullet, bullet_small_xpm) != 0)
    printf("bullet_small load failed, using fallback.\n");
  assets->loaded = true;
  return 0;
}

void player_view_destroy_assets(PlayerViewAssets *assets) {
  if (assets == NULL) return;
  sprite_destroy(&assets->p1);
  sprite_destroy(&assets->p2);
  sprite_destroy(&assets->bullet);
  assets->loaded = false;
}

static int draw_direction_indicator(const Player *player) {
  int front_x = player->position.x + (int)(PLAYER_INDICATOR_RADIUS * cosf(player->angle));
  int front_y = player->position.y + (int)(PLAYER_INDICATOR_RADIUS * sinf(player->angle));
  int ind_x = front_x - PLAYER_INDICATOR_SIZE / 2;
  int ind_y = front_y - PLAYER_INDICATOR_SIZE / 2;
  if (ind_x < 0 || ind_y < 0) return 0;
  return renderer_draw_rectangle((uint16_t)ind_x, (uint16_t)ind_y,
                                  PLAYER_INDICATOR_SIZE,
                                  PLAYER_INDICATOR_SIZE,
                                  PLAYER_INDICATOR_COLOR);
}

int player_view_draw(const Player *player, const PlayerViewAssets *assets, int player_num) {
  if (player == NULL) return 1;
  if (!player->alive) return 0;

  if (assets != NULL && assets->loaded) {
    const Sprite *sprite = (player_num == 1) ? &assets->p1 : &assets->p2;
    if (sprite_draw_rotated(sprite,
                            player->position.x,
                            player->position.y,
                            player->angle + PLAYER_SPRITE_ANGLE_OFFSET) != 0) return 1;
  } else {
    int x = player->position.x - player->width  / 2;
    int y = player->position.y - player->height / 2;
    if (x < 0 || y < 0) return 1;
    if (renderer_draw_rectangle((uint16_t)x, (uint16_t)y,
                                player->width, player->height,
                                player->color) != 0) return 1;
  }

  return draw_direction_indicator(player);
}

void player_view_draw_health_bar(const Player *player, int screen_x, int screen_y) {
  if (player == NULL || !player->alive || player->health <= 0) return;
  int hp_width  = 15;
  int bar_width = player->health * hp_width;
  int bar_height = 8;
  renderer_draw_rectangle(screen_x, screen_y, bar_width, bar_height, player->color);
}
