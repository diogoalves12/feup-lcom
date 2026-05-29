#ifndef PROJECT_PHYSICS_H
#define PROJECT_PHYSICS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int16_t x;
  int16_t y;
  uint16_t width;
  uint16_t height;
} BoundingBox;

bool physics_check_collision(BoundingBox a, BoundingBox b);
bool physics_check_collision_with_tile(BoundingBox box, int row, int col);

#endif /* PROJECT_PHYSICS_H */
