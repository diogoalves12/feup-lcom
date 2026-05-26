#ifndef PROJECT_GAME_H
#define PROJECT_GAME_H

#include <stdint.h>
#include "physics.h"
#include "arena.h"

typedef struct {
    BoundingBox box;
    int16_t vx;
    int16_t vy;
    uint32_t color;
} Player;

void game_move_player(Player *player, const Arena *arena);

#endif /* PROJECT_GAME_H */
