#include "game.h"
#include "arena.h"

void game_move_player(Player *player, const Arena *arena) {
    player->box.x += player->vx;
    
    int min_col = player->box.x / TILE_SIZE;
    int max_col = (player->box.x + player->box.width - 1) / TILE_SIZE;
    int min_row = player->box.y / TILE_SIZE;
    int max_row = (player->box.y + player->box.height - 1) / TILE_SIZE;

    for (int row = min_row; row <= max_row; row++) {
        for (int col = min_col; col <= max_col; col++) {
            TileType type = arena_get_tile_type(arena, row, col);
            if (arena_is_solid_tile(type)) {
                if (physics_check_collision_with_tile(player->box, row, col)) {
                    player->box.x -= player->vx;
                    goto end_x;
                }
            }
        }
    }
end_x:

    player->box.y += player->vy;

    min_col = player->box.x / TILE_SIZE;
    max_col = (player->box.x + player->box.width - 1) / TILE_SIZE;
    min_row = player->box.y / TILE_SIZE;
    max_row = (player->box.y + player->box.height - 1) / TILE_SIZE;

    for (int row = min_row; row <= max_row; row++) {
        for (int col = min_col; col <= max_col; col++) {
            TileType type = arena_get_tile_type(arena, row, col);
            if (arena_is_solid_tile(type)) {
                if (physics_check_collision_with_tile(player->box, row, col)) {
                    player->box.y -= player->vy;
                    goto end_y;
                }
            }
        }
    }
end_y:
    ;
}
