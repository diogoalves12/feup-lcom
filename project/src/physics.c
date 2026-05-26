#include "physics.h"
#include "arena.h"

bool physics_check_collision(BoundingBox a, BoundingBox b) {
    return (a.x < b.x + (int32_t)b.width &&
            a.x + (int32_t)a.width > b.x &&
            a.y < b.y + (int32_t)b.height &&
            a.y + (int32_t)a.height > b.y);
}

bool physics_check_collision_with_tile(BoundingBox box, int row, int col) {
    BoundingBox tile_box;
    tile_box.x = col * TILE_SIZE;
    tile_box.y = row * TILE_SIZE;
    tile_box.width = TILE_SIZE;
    tile_box.height = TILE_SIZE;
    
    return physics_check_collision(box, tile_box);
}
