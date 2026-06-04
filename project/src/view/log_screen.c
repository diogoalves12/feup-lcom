#include "log_screen.h"
#include "config.h"
#include "renderer.h"
#include <stddef.h>

#include "xpm/log/log.xpm"
#include "xpm/log/date.xpm"
#include "xpm/log/time.xpm"
#include "xpm/log/winner.xpm"
#include "xpm/log/slash.xpm"
#include "xpm/log/colon.xpm"
#include "xpm/log/dash.xpm"
#include "xpm/log/p1.xpm"
#include "xpm/log/p2.xpm"
#include "xpm/log/won.xpm"
#include "xpm/numbers/0.xpm"
#include "xpm/numbers/1.xpm"
#include "xpm/numbers/2.xpm"
#include "xpm/numbers/3.xpm"
#include "xpm/numbers/4.xpm"
#include "xpm/numbers/5.xpm"
#include "xpm/numbers/6.xpm"
#include "xpm/numbers/7.xpm"
#include "xpm/numbers/8.xpm"
#include "xpm/numbers/9.xpm"
#include "xpm/buttons/button_gray_wide.xpm"
#include "xpm/buttons/button_red_wide.xpm"
#include "xpm/text/back.xpm"
#include "xpm/cursor/pointer_b_shaded.xpm"

#define LOG_BG_COLOR 0x101010
#define LOG_BTN_Y 480
#define LOG_BTN_W 560
#define LOG_BTN_H 90
#define LOG_BTN_X ((SCREEN_WIDTH - LOG_BTN_W) / 2)

void log_screen_init(LogScreenState *state) {
  if (state == NULL) return;
  sprite_init(&state->title);
  sprite_init(&state->date_header);
  sprite_init(&state->time_header);
  sprite_init(&state->winner_header);
  for(int i=0; i<10; i++) sprite_init(&state->num[i]);
  sprite_init(&state->slash);
  sprite_init(&state->colon);
  sprite_init(&state->dash);
  sprite_init(&state->p1);
  sprite_init(&state->p2);
  sprite_init(&state->won);
  sprite_init(&state->button);
  sprite_init(&state->button_selected);
  sprite_init(&state->back_text);
  sprite_init(&state->cursor);
  state->cursor_x = SCREEN_WIDTH / 2;
  state->cursor_y = SCREEN_HEIGHT / 2;
  state->prev_lb = false;
  state->assets_loaded = false;
  state->hovering_back = false;
}

int log_screen_load_assets(LogScreenState *state) {
  if (state == NULL) return 1;
  if (sprite_load(&state->title, log_xpm) != 0) return 1;
  if (sprite_load(&state->date_header, date_xpm) != 0) return 1;
  if (sprite_load(&state->time_header, time_xpm) != 0) return 1;
  if (sprite_load(&state->winner_header, winner_xpm) != 0) return 1;
  
  if (sprite_load(&state->num[0], _0_xpm) != 0) return 1;
  if (sprite_load(&state->num[1], _1_xpm) != 0) return 1;
  if (sprite_load(&state->num[2], _2_xpm) != 0) return 1;
  if (sprite_load(&state->num[3], _3_xpm) != 0) return 1;
  if (sprite_load(&state->num[4], _4_xpm) != 0) return 1;
  if (sprite_load(&state->num[5], _5_xpm) != 0) return 1;
  if (sprite_load(&state->num[6], _6_xpm) != 0) return 1;
  if (sprite_load(&state->num[7], _7_xpm) != 0) return 1;
  if (sprite_load(&state->num[8], _8_xpm) != 0) return 1;
  if (sprite_load(&state->num[9], _9_xpm) != 0) return 1;
  
  if (sprite_load(&state->slash, slash_xpm) != 0) return 1;
  if (sprite_load(&state->colon, colon_xpm) != 0) return 1;
  if (sprite_load(&state->dash, dash_xpm) != 0) return 1;
  
  if (sprite_load(&state->p1, p1_xpm) != 0) return 1;
  if (sprite_load(&state->p2, p2_xpm) != 0) return 1;
  if (sprite_load(&state->won, won_xpm) != 0) return 1;
  
  if (sprite_load(&state->button, button_gray_wide) != 0) return 1;
  if (sprite_load(&state->button_selected, button_red_wide) != 0) return 1;
  if (sprite_load(&state->back_text, back) != 0) return 1;
  if (sprite_load(&state->cursor, pointer_b_shaded_xpm) != 0) return 1;
  
  state->assets_loaded = true;
  return 0;
}

void log_screen_destroy_assets(LogScreenState *state) {
  if (state == NULL) return;
  sprite_destroy(&state->title);
  sprite_destroy(&state->date_header);
  sprite_destroy(&state->time_header);
  sprite_destroy(&state->winner_header);
  for(int i=0; i<10; i++) sprite_destroy(&state->num[i]);
  sprite_destroy(&state->slash);
  sprite_destroy(&state->colon);
  sprite_destroy(&state->dash);
  sprite_destroy(&state->p1);
  sprite_destroy(&state->p2);
  sprite_destroy(&state->won);
  sprite_destroy(&state->button);
  sprite_destroy(&state->button_selected);
  sprite_destroy(&state->back_text);
  sprite_destroy(&state->cursor);
  state->assets_loaded = false;
}

static void log_screen_update_hover(LogScreenState *state) {
  int cx = (int)state->cursor_x;
  int cy = (int)state->cursor_y;
  state->hovering_back = (cx >= LOG_BTN_X && cx < LOG_BTN_X + LOG_BTN_W && cy >= LOG_BTN_Y && cy < LOG_BTN_Y + LOG_BTN_H);
}

void log_screen_move_cursor(LogScreenState *state, int16_t dx, int16_t dy) {
  if (state == NULL) return;
  int cur_w = (state->cursor.loaded && state->cursor.width > 0) ? (int)state->cursor.width : 1;
  int cur_h = (state->cursor.loaded && state->cursor.height > 0) ? (int)state->cursor.height : 1;
  int max_x = SCREEN_WIDTH - cur_w;
  int max_y = SCREEN_HEIGHT - cur_h;
  int candidate_x = (int)state->cursor_x + (int)dx;
  int candidate_y = (int)state->cursor_y - (int)dy;
  if (candidate_x < 0) candidate_x = 0; else if (candidate_x > max_x) candidate_x = max_x;
  if (candidate_y < 0) candidate_y = 0; else if (candidate_y > max_y) candidate_y = max_y;
  state->cursor_x = (int16_t)candidate_x;
  state->cursor_y = (int16_t)candidate_y;
  log_screen_update_hover(state);
}

void log_screen_apply_mouse(LogScreenState *state, const MouseInput *mouse, GameInputActions *actions) {
  if (state == NULL || mouse == NULL || actions == NULL) return;
  log_screen_update_hover(state);
  if (mouse->move_forward && !state->prev_lb && state->hovering_back) actions->back = true;
  state->prev_lb = mouse->move_forward;
}

void log_screen_update(LogScreenState *state, const GameInputActions *actions, GameState *next) {
  if (state == NULL || actions == NULL || next == NULL) return;
  if (actions->back || (actions->confirm && state->hovering_back)) {
    *next = GAME_STATE_MENU;
  }
}

static int draw_number(LogScreenState *state, uint8_t number, int x, int y) {
  uint8_t d1 = number / 10;
  uint8_t d2 = number % 10;
  if (sprite_draw(&state->num[d1], x, y) != 0) return 1;
  if (sprite_draw(&state->num[d2], x + state->num[d1].width + 2, y) != 0) return 1;
  return state->num[d1].width + state->num[d2].width + 4;
}

int log_screen_render(LogScreenState *state, const MatchLog *log) {
  if (state == NULL || !state->assets_loaded) return 1;
  if (renderer_clear(LOG_BG_COLOR) != 0) return 1;

  int title_x = (SCREEN_WIDTH - state->title.width) / 2;
  sprite_draw(&state->title, title_x, 30);

  int col1 = 150;
  int col2 = 350;
  int col3 = 550;

  sprite_draw(&state->date_header, col1, 90);
  sprite_draw(&state->time_header, col2, 90);
  sprite_draw(&state->winner_header, col3, 90);
  
  renderer_draw_hline(100, 120, 600, 0xFFFFFF);

  int y_start = 140;
  int y_step = 30;

  for (int i = 0; i < MAX_MATCH_LOGS; i++) {
    int y = y_start + i * y_step;
    
    if (i < log->count) {
      MatchLogEntry entry = log->entries[i];
      
      int cx = col1;
      cx += draw_number(state, entry.date.day, cx, y);
      sprite_draw(&state->slash, cx, y); cx += state->slash.width + 2;
      cx += draw_number(state, entry.date.month, cx, y);
      sprite_draw(&state->slash, cx, y); cx += state->slash.width + 2;
      cx += draw_number(state, entry.date.year, cx, y);
      
      cx = col2;
      cx += draw_number(state, entry.time.hours, cx, y);
      sprite_draw(&state->colon, cx, y); cx += state->colon.width + 2;
      cx += draw_number(state, entry.time.minutes, cx, y);
      sprite_draw(&state->colon, cx, y); cx += state->colon.width + 2;
      cx += draw_number(state, entry.time.seconds, cx, y);

      Sprite *p_spr = (entry.winner == 1) ? &state->p1 : &state->p2;
      sprite_draw(p_spr, col3, y);
      sprite_draw(&state->won, col3 + p_spr->width + 10, y);

    } else {
      sprite_draw(&state->dash, col1, y + 5);
      sprite_draw(&state->dash, col2, y + 5);
      sprite_draw(&state->dash, col3, y + 5);
    }
  }

  const Sprite *btn = state->hovering_back ? &state->button_selected : &state->button;
  sprite_draw(btn, LOG_BTN_X, LOG_BTN_Y);
  int bx = LOG_BTN_X + (LOG_BTN_W - state->back_text.width) / 2;
  int by = LOG_BTN_Y + (LOG_BTN_H - state->back_text.height) / 2;
  sprite_draw(&state->back_text, bx, by);

  sprite_draw_clipped(&state->cursor, state->cursor_x, state->cursor_y);
  return renderer_present();
}
