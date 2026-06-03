#include "collision.h"

#include <stddef.h>

typedef struct {
  int left;
  int right;
  int top;
  int bottom;
} PlayerBounds;

static PlayerBounds player_bounds_at(const Player *player, Position position) {
  PlayerBounds bounds;

  bounds.left = position.x - player->width / 2;
  bounds.right = position.x + player->width / 2 - 1;
  bounds.top = position.y - player->height / 2;
  bounds.bottom = position.y + player->height / 2 - 1;

  return bounds;
}

bool collision_player_walls(const Arena *arena, const Player *player, Position position) {
  if (arena == NULL || player == NULL) {
    return true;
  }

  const PlayerBounds bounds = player_bounds_at(player, position);

  if (bounds.left < 0 || bounds.top < 0) {
    return true;
  }

  const int left_col = bounds.left / TILE_SIZE;
  const int right_col = bounds.right / TILE_SIZE;
  const int top_row = bounds.top / TILE_SIZE;
  const int bottom_row = bounds.bottom / TILE_SIZE;

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
