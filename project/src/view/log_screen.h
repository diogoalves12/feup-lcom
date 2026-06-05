#ifndef PROJECT_VIEW_LOG_SCREEN_H
#define PROJECT_VIEW_LOG_SCREEN_H

#include <stdbool.h>
#include <stdint.h>

#include "game_input.h"
#include "game_state.h"
#include "match_log.h"
#include "mouse_input.h"
#include "sprite.h"

typedef struct {
  Sprite  button;
  Sprite  button_selected;
  Sprite  back_text;
  Sprite  cursor;
  int16_t cursor_x;
  int16_t cursor_y;
  bool    prev_lb;
  bool    back_hover;
  bool    assets_loaded;
} LogScreenState;

void log_screen_init(LogScreenState *state);
int  log_screen_load_assets(LogScreenState *state);
void log_screen_destroy_assets(LogScreenState *state);
void log_screen_reset(LogScreenState *state);
void log_screen_move_cursor(LogScreenState *state, int16_t dx, int16_t dy);
void log_screen_apply_mouse(LogScreenState *state, const MouseInput *mouse, GameInputActions *actions);
void log_screen_update(LogScreenState *state, const GameInputActions *actions, GameState *next);
int  log_screen_render(const LogScreenState *state, const MatchLog *log);

#endif
