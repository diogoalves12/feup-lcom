#include "player_view.h"

#include <math.h>
#include <stddef.h>

#include "renderer.h"

#include "xpm/player/player1/player_gun.xpm"
#include "xpm/player/player2/soldier1_gun.xpm"
#include "xpm/bullets/bullet_small.xpm"

#define PLAYER_INDICATOR_RADIUS 10
#define PLAYER_INDICATOR_SIZE 4
#define PLAYER_INDICATOR_COLOR 0xFFFFFF

#define PLAYER_HUD_BLOCK_W 18
#define PLAYER_HUD_BLOCK_H 12
#define PLAYER_HUD_GAP 3
#define PLAYER_HUD_PAD 4

int player_view_health_bar_width(void) {
  return PLAYER_DEFAULT_HEALTH * (PLAYER_HUD_BLOCK_W + PLAYER_HUD_GAP) - PLAYER_HUD_GAP + 2 * PLAYER_HUD_PAD;
}

int player_view_load_assets(PlayerViewAssets *assets) {
  if (assets == NULL) return 1;
  sprite_init(&assets->p1);
  sprite_init(&assets->p2);
  sprite_init(&assets->bullet);
  if (sprite_load(&assets->p1, player_gun_xpm) != 0) { player_view_destroy_assets(assets); return 1; }
  if (sprite_load(&assets->p2, soldier1_gun_xpm) != 0) { player_view_destroy_assets(assets); return 1; }
  if (sprite_load(&assets->bullet, bullet_small_xpm) != 0) { player_view_destroy_assets(assets); return 1; }
  return 0;
}

void player_view_destroy_assets(PlayerViewAssets *assets) {
  if (assets == NULL) return;
  sprite_destroy(&assets->p1);
  sprite_destroy(&assets->p2);
  sprite_destroy(&assets->bullet);
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
  if (player == NULL || assets == NULL) return 1;
  if (!player->alive) return 0;

  const Sprite *sprite = (player_num == 1) ? &assets->p1 : &assets->p2;
  if (sprite_draw_rotated(sprite,
                          player->position.x,
                          player->position.y,
                          player->angle) != 0) return 1;

  return draw_direction_indicator(player);
}

void player_view_draw_health_bar(const Player *player, int screen_x, int screen_y) {
  if (player == NULL) return;

  int bg_w = player_view_health_bar_width();
  int bg_h = PLAYER_HUD_BLOCK_H + 2 * PLAYER_HUD_PAD;

  renderer_draw_rectangle((uint16_t) screen_x, (uint16_t) screen_y,
                          (uint16_t) bg_w, (uint16_t) bg_h, 0x1A1A1A);

  renderer_draw_hline((uint16_t) screen_x, (uint16_t) screen_y, (uint16_t) bg_w, 0x555555);
  renderer_draw_hline((uint16_t) screen_x, (uint16_t)(screen_y + bg_h - 1), (uint16_t) bg_w, 0x555555);
  renderer_draw_rectangle((uint16_t) screen_x, (uint16_t) screen_y, 1, (uint16_t) bg_h, 0x555555);
  renderer_draw_rectangle((uint16_t)(screen_x + bg_w - 1), (uint16_t) screen_y, 1, (uint16_t) bg_h, 0x555555);

  int current_health = (player->alive && player->health > 0) ? player->health : 0;
  for (int i = 0; i < PLAYER_DEFAULT_HEALTH; i++) {
    int bx = screen_x + PLAYER_HUD_PAD + i * (PLAYER_HUD_BLOCK_W + PLAYER_HUD_GAP);
    int by = screen_y + PLAYER_HUD_PAD;
    uint32_t color = (i < current_health) ? player->color : 0x333333;
    renderer_draw_rectangle((uint16_t) bx, (uint16_t) by, PLAYER_HUD_BLOCK_W, PLAYER_HUD_BLOCK_H, color);
  }
}
