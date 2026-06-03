#include "combat.h"

#include <math.h>
#include <stddef.h>

void combat_init(CombatState *combat) {
  if (combat == NULL) return;

  combat->player1_next_allowed_shot_frame = 0;
  combat->player2_next_allowed_shot_frame = 0;
}

void combat_reset(CombatState *combat) {
  combat_init(combat);
}

static bool point_inside_player(int px, int py, const Player *player) {
  if (player == NULL || !player->alive) return false;

  int half_width = player->width / 2;
  int half_height = player->height / 2;
  int hitbox_left = player->position.x - half_width - COMBAT_AIM_TOLERANCE;
  int hitbox_right = player->position.x + half_width - 1 + COMBAT_AIM_TOLERANCE;
  int hitbox_top = player->position.y - half_height - COMBAT_AIM_TOLERANCE;
  int hitbox_bottom = player->position.y + half_height - 1 + COMBAT_AIM_TOLERANCE;

  return px >= hitbox_left && px <= hitbox_right && py >= hitbox_top  && py <= hitbox_bottom;
}

bool combat_try_shoot(CombatState *combat, int shooter_num, const Player *shooter, Player *target, const Arena *arena, uint32_t frame_counter) {
  if (combat == NULL || shooter == NULL || target == NULL || arena == NULL) return false;
  if (shooter_num != 1 && shooter_num != 2) return false;

  uint32_t *next_allowed_shot_frame = (shooter_num == 1) ? &combat->player1_next_allowed_shot_frame : &combat->player2_next_allowed_shot_frame;

  if (frame_counter < *next_allowed_shot_frame) return false;
  if (!shooter->alive || !target->alive)        return false;

  *next_allowed_shot_frame = frame_counter + COMBAT_SHOT_COOLDOWN_TICKS;

  float direction_x = cosf(shooter->angle);
  float direction_y = sinf(shooter->angle);
  float step_x = direction_x * (float) COMBAT_RAY_STEP;
  float step_y = direction_y * (float) COMBAT_RAY_STEP;

  float shooter_half_width = (float) (shooter->width / 2);
  float ray_x = (float) shooter->position.x + direction_x * (shooter_half_width + 1.0f);
  float ray_y = (float) shooter->position.y + direction_y * (shooter_half_width + 1.0f);

  const int arena_max_x = ARENA_COLS * TILE_SIZE;
  const int arena_max_y = ARENA_ROWS * TILE_SIZE;
  int travelled_distance = 0;

  while (true) {
    int pixel_x = (int) ray_x;
    int pixel_y = (int) ray_y;

    if (pixel_x < 0 || pixel_y < 0 || pixel_x >= arena_max_x || pixel_y >= arena_max_y) return false;
    if (travelled_distance > COMBAT_MAX_DISTANCE) return false;

    int tile_col = pixel_x / TILE_SIZE;
    int tile_row = pixel_y / TILE_SIZE;
    if (arena_is_wall_tile(arena_get_tile_type(arena, tile_row, tile_col))) return false;

    if (point_inside_player(pixel_x, pixel_y, target)) {
      player_damage(target, COMBAT_DAMAGE);
      return true;
    }

    ray_x += step_x;
    ray_y += step_y;
    travelled_distance += COMBAT_RAY_STEP;
  }
}
