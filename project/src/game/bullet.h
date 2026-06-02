#ifndef PROJECT_GAME_BULLET_H
#define PROJECT_GAME_BULLET_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "player.h"

#define MAX_BULLETS        16
#define BULLET_SPEED       6.0f
#define BULLET_SIZE        4
#define BULLET_DAMAGE      1
#define BULLET_SHOOT_COOLDOWN 20
#define BULLET_COLOR       0xFFFFFF

typedef struct {
  float x;
  float y;
  float angle;
  bool  active;
  int   owner; /* 1 or 2 */
} Bullet;

typedef struct {
  Bullet bullets[MAX_BULLETS];
  int    p1_cooldown;
  int    p2_cooldown;
} BulletSystem;

void bullet_system_init(BulletSystem *bs);
void bullet_system_reset(BulletSystem *bs);
void bullet_system_shoot(BulletSystem *bs, const Player *shooter, int player_num);
void bullet_system_update(BulletSystem *bs, const Arena *arena, Player *p1, Player *p2);
int  bullet_system_draw(const BulletSystem *bs);

#endif
