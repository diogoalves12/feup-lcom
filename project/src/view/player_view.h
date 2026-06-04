#ifndef PROJECT_VIEW_PLAYER_VIEW_H
#define PROJECT_VIEW_PLAYER_VIEW_H

#include <stdbool.h>

#include "player.h"
#include "sprite.h"

typedef struct {
  Sprite p1;
  Sprite p2;
  Sprite bullet;
  bool loaded;
} PlayerViewAssets;

int player_view_load_assets(PlayerViewAssets *assets);
void player_view_destroy_assets(PlayerViewAssets *assets);
int player_view_draw(const Player *player, const PlayerViewAssets *assets, int player_num);
void player_view_draw_health_bar(const Player *player, int screen_x, int screen_y);
int  player_view_health_bar_width(void);

#endif
