#include "arena_layout.h"

// Places a wall rectangle.
static void add_rect(Arena *arena, Rect rect) {
  for (int row = rect.row; row < rect.row + rect.height; row++) {
    for (int col = rect.col; col < rect.col + rect.width; col++) {
      arena_set_tile(arena, row, col, TILE_WALL);
    }
  }
}

// Applies the easy layout.
static void apply_easy(Arena *arena) {
  add_rect(arena, (Rect) {5, 10, 2, 2});
  add_rect(arena, (Rect) {17, 10, 2, 2});
  add_rect(arena, (Rect) {10, 15, 1, 3});
  add_rect(arena, (Rect) {10, 16, 1, 3});
  add_rect(arena, (Rect) {8, 24, 2, 2});
  add_rect(arena, (Rect) {14, 6, 2, 2});
}

// Applies the medium layout.
static void apply_medium(Arena *arena) {
  add_rect(arena, (Rect) {4, 8, 2, 3});
  add_rect(arena, (Rect) {17, 8, 2, 3});
  add_rect(arena, (Rect) {7, 14, 1, 4});
  add_rect(arena, (Rect) {13, 17, 1, 4});
  add_rect(arena, (Rect) {10, 12, 2, 2});
  add_rect(arena, (Rect) {12, 18, 2, 2});
  add_rect(arena, (Rect) {6, 24, 2, 2});
  add_rect(arena, (Rect) {16, 24, 2, 2});
  add_rect(arena, (Rect) {9, 5, 2, 1});
  add_rect(arena, (Rect) {14, 5, 2, 1});
}

// Applies the hard layout.
static void apply_hard(Arena *arena) {
  add_rect(arena, (Rect) {4, 7, 2, 3});
  add_rect(arena, (Rect) {17, 7, 2, 3});
  add_rect(arena, (Rect) {6, 12, 1, 4});
  add_rect(arena, (Rect) {14, 12, 1, 4});
  add_rect(arena, (Rect) {5, 18, 2, 2});
  add_rect(arena, (Rect) {17, 18, 2, 2});
  add_rect(arena, (Rect) {10, 10, 2, 1});
  add_rect(arena, (Rect) {13, 10, 2, 1});
  add_rect(arena, (Rect) {9, 16, 2, 1});
  add_rect(arena, (Rect) {14, 16, 2, 1});
  add_rect(arena, (Rect) {10, 22, 1, 3});
  add_rect(arena, (Rect) {11, 23, 1, 3});
}

void arena_layout_apply(Arena *arena, ArenaDifficulty difficulty) {
  switch (difficulty) {
    case ARENA_EASY:
      apply_easy(arena);
      break;
    case ARENA_HARD:
      apply_hard(arena);
      break;
    case ARENA_MEDIUM:
    default:
      apply_medium(arena);
      break;
  }
}
