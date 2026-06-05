/**
 * @file types.h
 * @brief Small geometry types shared by the model.
 */
#ifndef TYPES_H
#define TYPES_H

/**
 * @brief Pixel position.
 */
typedef struct {
  int x; /**< Horizontal coordinate (pixels from the left edge). */
  int y; /**< Vertical coordinate (pixels from the top edge). */
} Position;

/**
 * @brief Tile grid position.
 */
typedef struct {
  int row; /**< Row index (0 = top row). */
  int col; /**< Column index (0 = left column). */
} GridPosition;

/**
 * @brief Rectangle in tile units.
 */
typedef struct {
  int row;    /**< Top-left row. */
  int col;    /**< Top-left column. */
  int width;  /**< Width in tiles. */
  int height; /**< Height in tiles. */
} Rect;

#endif
