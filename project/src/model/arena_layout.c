#include "arena_layout.h"

static void add_rect(Arena *arena, Rect rect) {
  for (int row = rect.row; row < rect.row + rect.height; row++) {
    for (int col = rect.col; col < rect.col + rect.width; col++) {
      arena_set_tile(arena, row, col, TILE_WALL);
    }
  }
}

static void apply_easy(Arena *arena) {
  add_rect(arena, (Rect) {6, 10, 2, 2});
  add_rect(arena, (Rect) {22, 10, 2, 2});
  add_rect(arena, (Rect) {12, 19, 4, 1});
  add_rect(arena, (Rect) {17, 19, 4, 1});
  add_rect(arena, (Rect) {10, 28, 2, 2});
  add_rect(arena, (Rect) {18, 28, 2, 2});
}

static void apply_medium(Arena *arena) {
  add_rect(arena, (Rect) {5, 8, 3, 2});
  add_rect(arena, (Rect) {23, 8, 3, 2});
  add_rect(arena, (Rect) {8, 17, 4, 1});
  add_rect(arena, (Rect) {21, 17, 4, 1});
  add_rect(arena, (Rect) {13, 14, 2, 2});
  add_rect(arena, (Rect) {15, 25, 2, 2});
  add_rect(arena, (Rect) {7, 29, 2, 2});
  add_rect(arena, (Rect) {21, 29, 2, 2});
  add_rect(arena, (Rect) {11, 5, 1, 2});
  add_rect(arena, (Rect) {17, 5, 1, 2});
}

static void apply_hard(Arena *arena) {
  add_rect(arena, (Rect) {4, 7, 3, 2});
  add_rect(arena, (Rect) {24, 7, 3, 2});
  add_rect(arena, (Rect) {7, 14, 4, 1});
  add_rect(arena, (Rect) {22, 14, 4, 1});
  add_rect(arena, (Rect) {5, 22, 2, 2});
  add_rect(arena, (Rect) {23, 22, 2, 2});
  add_rect(arena, (Rect) {12, 11, 1, 2});
  add_rect(arena, (Rect) {16, 11, 1, 2});
  add_rect(arena, (Rect) {11, 18, 1, 2});
  add_rect(arena, (Rect) {17, 18, 1, 2});
  add_rect(arena, (Rect) {12, 28, 3, 1});
  add_rect(arena, (Rect) {17, 28, 3, 1});
  add_rect(arena, (Rect) {9, 33, 2, 2});
  add_rect(arena, (Rect) {19, 33, 2, 2});
}

void arena_layout_apply(Arena *arena, ArenaDifficulty difficulty) {
  switch (difficulty) {
    case ARENA_EASY:   apply_easy(arena);   break;
    case ARENA_HARD:   apply_hard(arena);   break;
    case ARENA_MEDIUM:
    default:           apply_medium(arena); break;
  }
}
