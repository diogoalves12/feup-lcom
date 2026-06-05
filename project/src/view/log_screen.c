#include "log_screen.h"

#include <stddef.h>
#include <stdio.h>

#include "config.h"
#include "renderer.h"
#include "text.h"

#include "xpm/buttons/button_gray_wide.xpm"
#include "xpm/buttons/button_red_wide.xpm"
#include "xpm/cursor/pointer_b_shaded.xpm"
#include "xpm/text/back.xpm"

#define LOG_BG_COLOR       0x101010
#define LOG_TEXT_COLOR     0xE0E0E0
#define LOG_TITLE_COLOR    0xFFD000
#define LOG_EMPTY_COLOR    0x808080

#define LOG_BTN_H  90
#define LOG_BACK_Y 480

#define LOG_TITLE_Y       30
#define LOG_LIST_START_Y 110
#define LOG_LIST_STEP_Y   60
#define LOG_LIST_X        60

#define LOG_TEXT_SCALE  3
#define LOG_TITLE_SCALE 5

static int log_back_btn_x(const LogScreenState *state) {
  return (SCREEN_WIDTH - (int)state->button.width) / 2;
}

static int log_back_btn_w(const LogScreenState *state) {
  return (int)state->button.width;
}

void log_screen_init(LogScreenState *state) {
  if (state == NULL) return;
  sprite_init(&state->button);
  sprite_init(&state->button_selected);
  sprite_init(&state->back_text);
  sprite_init(&state->cursor);
  state->cursor_x = SCREEN_WIDTH / 2;
  state->cursor_y = LOG_BACK_Y + LOG_BTN_H / 2;
  state->prev_lb = false;
  state->back_hover = false;
}

int log_screen_load_assets(LogScreenState *state) {
  if (state == NULL) return 1;

  if (sprite_load(&state->button, button_gray_wide) != 0) { log_screen_destroy_assets(state); return 1; }
  if (sprite_load(&state->button_selected, button_red_wide) != 0) { log_screen_destroy_assets(state); return 1; }
  if (sprite_load(&state->back_text, back) != 0) { log_screen_destroy_assets(state); return 1; }
  if (sprite_load(&state->cursor, pointer_b_shaded_xpm) != 0) { log_screen_destroy_assets(state); return 1; }

  return 0;
}

void log_screen_destroy_assets(LogScreenState *state) {
  if (state == NULL) return;
  sprite_destroy(&state->button);
  sprite_destroy(&state->button_selected);
  sprite_destroy(&state->back_text);
  sprite_destroy(&state->cursor);
}

void log_screen_reset(LogScreenState *state) {
  if (state == NULL) return;
  state->cursor_x = SCREEN_WIDTH / 2;
  state->cursor_y = LOG_BACK_Y + LOG_BTN_H / 2;
  state->prev_lb = false;
  state->back_hover = true;
}

static void log_screen_update_hover(LogScreenState *state) {
  int cx = (int)state->cursor_x;
  int cy = (int)state->cursor_y;

  int bx = log_back_btn_x(state);
  int bw = log_back_btn_w(state);

  state->back_hover = (cx >= bx && cx < bx + bw &&
                      cy >= LOG_BACK_Y && cy < LOG_BACK_Y + LOG_BTN_H);
}

void log_screen_move_cursor(LogScreenState *state, int16_t dx, int16_t dy) {
  if (state == NULL) return;

  int cur_w = (state->cursor.loaded && state->cursor.width > 0) ? (int)state->cursor.width : 1;
  int cur_h = (state->cursor.loaded && state->cursor.height > 0) ? (int)state->cursor.height : 1;
  int max_x = SCREEN_WIDTH - cur_w;
  int max_y = SCREEN_HEIGHT - cur_h;

  int candidate_x = (int)state->cursor_x + (int)dx;
  int candidate_y = (int)state->cursor_y - (int)dy;

  if (candidate_x < 0) candidate_x = 0;
  else if (candidate_x > max_x) candidate_x = max_x;

  if (candidate_y < 0) candidate_y = 0;
  else if (candidate_y > max_y) candidate_y = max_y;

  state->cursor_x = (int16_t)candidate_x;
  state->cursor_y = (int16_t)candidate_y;

  log_screen_update_hover(state);
}

void log_screen_apply_mouse(LogScreenState *state, const MouseInput *mouse, GameInputActions *actions) {
  if (state == NULL || mouse == NULL || actions == NULL) return;
  log_screen_update_hover(state);
  if (mouse->move_forward && !state->prev_lb && state->back_hover) {
    actions->confirm = true;
  }
  state->prev_lb = mouse->move_forward;
}

void log_screen_update(LogScreenState *state, const GameInputActions *actions, GameState *next) {
  if (state == NULL || actions == NULL || next == NULL) return;
  if (actions->confirm || actions->back) {
    *next = GAME_STATE_MENU;
  }
}

static const char *difficulty_label(ArenaDifficulty diff) {
  switch (diff) {
    case ARENA_EASY:   return "EASY";
    case ARENA_MEDIUM: return "MEDIUM";
    case ARENA_HARD:   return "HARD";
  }
  return "?";
}

static void format_entry(const MatchLogEntry *entry, char *buffer, size_t buffer_size) {
  const char *winner_str = (entry->winner == 1) ? "P1" :
                           (entry->winner == 2) ? "P2" : "--";
  const char *diff_str = difficulty_label(entry->difficulty);

  if (entry->valid_timestamp) {
    snprintf(buffer, buffer_size, "%02u/%02u/%04u  %02u:%02u  %s  %s",
             (unsigned) entry->timestamp.day,
             (unsigned) entry->timestamp.month,
             (unsigned) entry->timestamp.year,
             (unsigned) entry->timestamp.hour,
             (unsigned) entry->timestamp.minute,
             winner_str, diff_str);
  } else {
    snprintf(buffer, buffer_size, "--/--/----  --:--  %s  %s", winner_str, diff_str);
  }
}

static int draw_back_button(const LogScreenState *state) {
  bool selected = state->back_hover;
  const Sprite *btn = selected ? &state->button_selected : &state->button;
  uint16_t bx = (uint16_t)((SCREEN_WIDTH - (int)btn->width) / 2);
  uint16_t by = (uint16_t)(LOG_BACK_Y + ((int)LOG_BTN_H - (int)btn->height) / 2);
  if (sprite_draw(btn, bx, by) != 0) return 1;

  int lx = ((int)SCREEN_WIDTH - (int)state->back_text.width) / 2;
  int ly = (int)LOG_BACK_Y + ((int)LOG_BTN_H - (int)state->back_text.height) / 2;
  if (lx < 0) lx = 0;
  if (ly < 0) ly = 0;
  if (sprite_draw(&state->back_text, (uint16_t)lx, (uint16_t)ly) != 0) return 1;
  return 0;
}

int log_screen_render(const LogScreenState *state, const MatchLog *log) {
  if (state == NULL || log == NULL) return 1;

  if (renderer_clear(LOG_BG_COLOR) != 0) return 1;

  const char *title = "MATCH LOG";
  int title_w = text_string_width(title, LOG_TITLE_SCALE);
  text_draw((SCREEN_WIDTH - title_w) / 2, LOG_TITLE_Y, title, LOG_TITLE_SCALE, LOG_TITLE_COLOR);

  if (log->count == 0) {
    const char *empty = "NO MATCHES YET";
    int ew = text_string_width(empty, LOG_TEXT_SCALE);
    int ey = LOG_LIST_START_Y + (LOG_LIST_STEP_Y * 2);
    text_draw((SCREEN_WIDTH - ew) / 2, ey, empty, LOG_TEXT_SCALE, LOG_EMPTY_COLOR);
  } else {
    char line[64];
    for (int i = 0; i < log->count; i++) {
      const MatchLogEntry *entry = &log->entries[i];
      if (!entry->valid) continue;
      format_entry(entry, line, sizeof(line));
      int y = LOG_LIST_START_Y + i * LOG_LIST_STEP_Y;
      text_draw(LOG_LIST_X, y, line, LOG_TEXT_SCALE, LOG_TEXT_COLOR);
    }
  }

  if (draw_back_button(state) != 0) return 1;

  if (sprite_draw_clipped(&state->cursor, state->cursor_x, state->cursor_y) != 0) return 1;

  return renderer_present();
}
