#ifndef PROJECT_GAME_TYPES_H
#define PROJECT_GAME_TYPES_H

typedef struct {
  int x;
  int y;
} Position;

typedef struct {
  int row;
  int col;
} GridPosition;

typedef struct {
  int row;
  int col;
  int width;
  int height;
} Rect;

#endif
