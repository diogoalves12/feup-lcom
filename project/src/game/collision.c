#include "collision.h"

#include <stddef.h>

bool collision_player_walls(const Arena *arena, const Player *player, Position position) {
  if (arena == NULL || player == NULL) {
    return true;
  }

  int left = position.x - player->width / 2;
  int right = position.x + player->width / 2 - 1;
  int top = position.y - player->height / 2;
  int bottom = position.y + player->height / 2 - 1;

  if (left < 0 || top < 0) {
    return true;
  }

  int left_col = left / TILE_SIZE;
  int right_col = right / TILE_SIZE;
  int top_row = top / TILE_SIZE;
  int bottom_row = bottom / TILE_SIZE;

  for (int row = top_row; row <= bottom_row; row++) {
    for (int col = left_col; col <= right_col; col++) {
      TileType type = arena_get_tile_type(arena, row, col);

      if (arena_is_wall_tile(type)) {
        return true;
      }
    }
  }

  return false;
}
