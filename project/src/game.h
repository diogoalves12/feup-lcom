#ifndef PROJECT_GAME_H
#define PROJECT_GAME_H

#include <stdint.h>
#include "physics.h"

typedef struct {
    BoundingBox box;
    int16_t vx;
    int16_t vy;
    uint32_t color;
} Player;

typedef struct {
    BoundingBox box;
    uint32_t color;
} Wall;

void game_move_player(Player *player, const Wall *walls, uint32_t num_walls);

#endif /* PROJECT_GAME_H */
