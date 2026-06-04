#ifndef PROJECT_VIEW_LOG_SCREEN_H
#define PROJECT_VIEW_LOG_SCREEN_H

#include "match_log.h"
#include "sprite.h"
#include "game_input.h"
#include "game_state.h"

typedef struct {
  Sprite title;
  Sprite date_header;
  Sprite time_header;
  Sprite winner_header;
  
  Sprite num[10];
  Sprite slash;
  Sprite colon;
  Sprite dash;
  
  Sprite p1;
  Sprite p2;
  Sprite won;
  
  Sprite button;
  Sprite button_selected;
  Sprite back_text;
  
  Sprite cursor;
  int16_t cursor_x;
  int16_t cursor_y;
  
  bool prev_lb;
  bool assets_loaded;
  bool hovering_back;
} LogScreenState;

void log_screen_init(LogScreenState *state);
int log_screen_load_assets(LogScreenState *state);
void log_screen_destroy_assets(LogScreenState *state);
void log_screen_move_cursor(LogScreenState *state, int16_t dx, int16_t dy);
void log_screen_apply_mouse(LogScreenState *state, const MouseInput *mouse, GameInputActions *actions);
void log_screen_update(LogScreenState *state, const GameInputActions *actions, GameState *next);
int log_screen_render(LogScreenState *state, const MatchLog *log);

#endif
