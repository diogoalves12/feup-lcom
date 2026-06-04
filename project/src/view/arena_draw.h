#ifndef PROJECT_VIEW_ARENA_DRAW_H
#define PROJECT_VIEW_ARENA_DRAW_H

#include <stdint.h>

#include "arena.h"

int  arena_view_load_assets(void);
void arena_view_destroy_assets(void);
int  arena_view_draw(const Arena *arena, uint32_t frame_counter);

#endif
