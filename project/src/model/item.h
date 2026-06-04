#ifndef PROJECT_MODEL_ITEM_H
#define PROJECT_MODEL_ITEM_H

#include <stdbool.h>
#include <stdint.h>

#include "arena.h"
#include "player.h"
#include "types.h"

#define MAX_ITEMS 8
#define HEALTH_PICKUP_HEAL_AMOUNT 1
#define HEALTH_PICKUP_RESPAWN_FRAMES 600
#define HEALTH_PICKUP_FRAME_COUNT 4

typedef enum {
  ITEM_HEALTH = 0
} ItemType;

typedef struct {
  ItemType type;
  Position position;
  bool     active;
  uint32_t respawn_frame;
} Item;

typedef struct {
  Item items[MAX_ITEMS];
  int  count;
} ItemManager;

void item_manager_init(ItemManager *manager, ArenaDifficulty difficulty);
void item_manager_update(ItemManager *manager, Player *player1, Player *player2, uint32_t frame_counter);

#endif
