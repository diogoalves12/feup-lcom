#include "physics.h"

bool physics_check_collision(BoundingBox a, BoundingBox b) {
    return (a.x < b.x + (int32_t)b.width &&
            a.x + (int32_t)a.width > b.x &&
            a.y < b.y + (int32_t)b.height &&
            a.y + (int32_t)a.height > b.y);
}
