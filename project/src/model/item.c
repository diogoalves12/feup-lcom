#include "item.h"

#include <stddef.h>

static int abs_int(int v) {
  return v < 0 ? -v : v;
}

static void add_item(ItemPool *pool, ItemType type, int tile_row, int tile_col) {
  if (pool->count >= MAX_ITEMS) return;
  Item *it = &pool->items[pool->count++];
  it->type = type;
  it->position.x = tile_col * TILE_SIZE + TILE_SIZE / 2;
  it->position.y = tile_row * TILE_SIZE + TILE_SIZE / 2;
  it->active = true;
  it->respawn_frame = 0;
}

void item_pool_init(ItemPool *pool, ArenaDifficulty difficulty) {
  if (pool == NULL) return;
  pool->count = 0;

  switch (difficulty) {
    case ARENA_EASY:
      add_item(pool, ITEM_HEALTH, 15, 20);
      break;
    case ARENA_HARD:
      add_item(pool, ITEM_HEALTH, 5, 20);
      add_item(pool, ITEM_HEALTH, 24, 20);
      add_item(pool, ITEM_HEALTH, 15, 6);
      add_item(pool, ITEM_HEALTH, 15, 33);
      break;
    case ARENA_MEDIUM:
    default:
      add_item(pool, ITEM_HEALTH, 6, 20);
      add_item(pool, ITEM_HEALTH, 23, 20);
      break;
  }
}

static bool item_touches_player(const Item *item, const Player *player) {
  if (item == NULL || !item->active) return false;
  if (player == NULL || !player->alive) return false;

  int item_half = TILE_SIZE / 2;
  int player_half_w = player->width / 2;
  int player_half_h = player->height / 2;

  return abs_int(item->position.x - player->position.x) <= (item_half + player_half_w) &&
         abs_int(item->position.y - player->position.y) <= (item_half + player_half_h);
}

static bool try_consume(Item *item, Player *player) {
  if (!item_touches_player(item, player)) return false;

  if (item->type == ITEM_HEALTH) {
    if (player->health >= PLAYER_DEFAULT_HEALTH) return false;
    player_heal(player, HEALTH_PICKUP_HEAL_AMOUNT);
    return true;
  }
  return false;
}

void item_pool_update(ItemPool *pool, Player *player1, Player *player2, uint32_t frame_counter) {
  if (pool == NULL) return;

  for (int i = 0; i < pool->count; i++) {
    Item *item = &pool->items[i];

    if (item->active) {
      bool taken = try_consume(item, player1);
      if (!taken) taken = try_consume(item, player2);
      if (taken) {
        item->active = false;
        item->respawn_frame = frame_counter + HEALTH_PICKUP_RESPAWN_FRAMES;
      }
    } else {
      if (frame_counter >= item->respawn_frame) {
        item->active = true;
      }
    }
  }
}
