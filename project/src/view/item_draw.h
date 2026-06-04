#ifndef PROJECT_VIEW_ITEM_DRAW_H
#define PROJECT_VIEW_ITEM_DRAW_H

#include <stdint.h>

#include "item.h"

int  item_view_load_assets(void);
void item_view_destroy_assets(void);
int  item_view_draw(const ItemManager *manager, uint32_t frame_counter);

#endif
