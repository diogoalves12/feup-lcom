#include "bullet.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>

#include "renderer.h"

static bool bullet_hits_wall(const Arena *arena, float x, float y) {
  int col = (int) x / TILE_SIZE;
  int row = (int) y / TILE_SIZE;
  return arena_is_wall_tile(arena_get_tile_type(arena, row, col));
}

static bool bullet_hits_player(const Bullet *b, const Player *player) {
  if (!player->alive) return false;

  int bx = (int) b->x;
  int by = (int) b->y;
  int bl = bx;
  int br = bx + BULLET_SIZE - 1;
  int bt = by;
  int bb = by + BULLET_SIZE - 1;

  int pl = player->position.x - player->width  / 2;
  int pr = player->position.x + player->width  / 2 - 1;
  int pt = player->position.y - player->height / 2;
  int pb = player->position.y + player->height / 2 - 1;

  return bl <= pr && br >= pl && bt <= pb && bb >= pt;
}

void bullet_system_init(BulletSystem *bs) {
  if (bs == NULL) return;
  for (int i = 0; i < MAX_BULLETS; i++) {
    bs->bullets[i].active = false;
  }
  bs->p1_cooldown = 0;
  bs->p2_cooldown = 0;
}

void bullet_system_reset(BulletSystem *bs) {
  bullet_system_init(bs);
}

void bullet_system_shoot(BulletSystem *bs, const Player *shooter, int player_num) {
  if (bs == NULL || shooter == NULL || !shooter->alive) return;

  int *cooldown = (player_num == 1) ? &bs->p1_cooldown : &bs->p2_cooldown;
  if (*cooldown > 0) return;

  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bs->bullets[i].active) {
      /* Spawn at the front edge of the player. */
      float half = (float) (shooter->width / 2);
      bs->bullets[i].x      = (float) shooter->position.x + cosf(shooter->angle) * half;
      bs->bullets[i].y      = (float) shooter->position.y + sinf(shooter->angle) * half;
      bs->bullets[i].angle  = shooter->angle;
      bs->bullets[i].active = true;
      bs->bullets[i].owner  = player_num;
      *cooldown = BULLET_SHOOT_COOLDOWN;
      printf("[bullet] p%d shot: x=%.1f y=%.1f angle=%.2f slot=%d\n",
             player_num, bs->bullets[i].x, bs->bullets[i].y, bs->bullets[i].angle, i);
      return;
    }
  }
}

void bullet_system_update(BulletSystem *bs, const Arena *arena, Player *p1, Player *p2) {
  if (bs == NULL) return;

  if (bs->p1_cooldown > 0) bs->p1_cooldown--;
  if (bs->p2_cooldown > 0) bs->p2_cooldown--;

  for (int i = 0; i < MAX_BULLETS; i++) {
    Bullet *b = &bs->bullets[i];
    if (!b->active) continue;

    b->x += cosf(b->angle) * BULLET_SPEED;
    b->y += sinf(b->angle) * BULLET_SPEED;

    if (b->x < 0 || b->y < 0
        || b->x >= (float) (ARENA_COLS * TILE_SIZE)
        || b->y >= (float) (ARENA_ROWS * TILE_SIZE)) {
      b->active = false;
      continue;
    }

    if (bullet_hits_wall(arena, b->x, b->y)) {
      b->active = false;
      continue;
    }

    /* A bullet cannot hit its owner. */
    if (b->owner != 1 && bullet_hits_player(b, p1)) {
      player_damage(p1, BULLET_DAMAGE);
      b->active = false;
      continue;
    }

    if (b->owner != 2 && bullet_hits_player(b, p2)) {
      player_damage(p2, BULLET_DAMAGE);
      b->active = false;
    }
  }
}

int bullet_system_draw(const BulletSystem *bs) {
  if (bs == NULL) return 1;

  for (int i = 0; i < MAX_BULLETS; i++) {
    const Bullet *b = &bs->bullets[i];
    if (!b->active) continue;

    int x = (int) b->x;
    int y = (int) b->y;
    if (x < 0 || y < 0) continue;

    if (renderer_draw_rectangle((uint16_t) x, (uint16_t) y, BULLET_SIZE, BULLET_SIZE, BULLET_COLOR) != 0) {
      return 1;
    }
  }

  return 0;
}
