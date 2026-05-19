#include "game.h"

void game_move_player(Player *player, const Wall *walls, uint32_t num_walls) {
    player->box.x += player->vx;
    
    for (uint32_t i = 0; i < num_walls; i++) {
        if (physics_check_collision(player->box, walls[i].box)) {
            player->box.x -= player->vx;
            break;
        }
    }

    player->box.y += player->vy;

    for (uint32_t i = 0; i < num_walls; i++) {
        if (physics_check_collision(player->box, walls[i].box)) {
            player->box.y -= player->vy;
            break;
        }
    }
}
