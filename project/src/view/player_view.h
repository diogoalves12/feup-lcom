#ifndef PROJECT_VIEW_PLAYER_VIEW_H
#define PROJECT_VIEW_PLAYER_VIEW_H

#include "player.h"

int  player_view_draw(const Player *player);
void player_view_draw_health_bar(const Player *player, int screen_x, int screen_y);

#endif
