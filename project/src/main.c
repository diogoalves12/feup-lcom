#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "arena.h"
#include "collision.h"
#include "game_input.h"
#include "game_over.h"
#include "game_state.h"
#include "i8042.h"
#include "keyboard.h"
#include "keyboard_input.h"
#include "menu.h"
#include "player.h"
#include "renderer.h"

#define PROJECT_VIDEO_MODE 0x115
#define PROJECT_BG_COLOR   0x101010

/* Yellow bar drawn at the top of the arena to indicate pause. */
#define PAUSE_BAR_COLOR  0xFFFF00
#define PAUSE_BAR_HEIGHT 4
#define ARENA_PIXEL_WIDTH  (ARENA_COLS * TILE_SIZE)

typedef struct {
  GameState        state;
  uint32_t         frame_counter;
  KeyboardInput    keyboard;
  GameInputActions actions;
  Arena            arena;
  Player           player1;
  Player           player2;
  MenuState        menu;
  GameOverState    game_over;
} Game;

/* ------------------------------------------------------------------ */
/* Forward declarations                                                */
/* ------------------------------------------------------------------ */

static int  game_init(Game *game);
static int  game_run(Game *game);
static int  game_shutdown(Game *game);

static void game_start_match(Game *game);
static void game_apply_transition(Game *game, GameState next);
static void game_tick(Game *game);

static void state_menu_tick(Game *game);
static void state_playing_tick(Game *game);
static void state_paused_tick(Game *game);
static void state_game_over_tick(Game *game);

static int  render_playing(const Game *game);
static int  render_paused(const Game *game);

/* ------------------------------------------------------------------ */
/* Entry points                                                        */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/project/trace.txt");
  lcf_log_output("/home/lcom/labs/project/output.txt");
  if (lcf_start(argc, argv)) return 1;
  lcf_cleanup();
  return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  Game game;

  if (game_init(&game) != 0) return 1;

  int result = game_run(&game);

  if (game_shutdown(&game) != 0) {
    printf("game_shutdown failed.\n");
    return 1;
  }

  return result;
}

/* ------------------------------------------------------------------ */
/* Initialization and shutdown                                         */
/* ------------------------------------------------------------------ */

static int game_init(Game *game) {
  if (game == NULL) return 1;

  if (renderer_init(PROJECT_VIDEO_MODE) != 0) {
    printf("renderer_init failed for mode 0x%03X.\n", PROJECT_VIDEO_MODE);
    return 1;
  }

  keyboard_input_init(&game->keyboard);
  game_input_actions_init(&game->actions);
  game->frame_counter      = 0;
  game->game_over.winner   = 0;
  game->game_over.selection = GAME_OVER_SEL_RESTART;

  game->state = GAME_STATE_MENU;
  menu_state_init(&game->menu);

  return 0;
}

static int game_shutdown(Game *game) {
  (void) game;
  if (renderer_shutdown() != 0) {
    printf("renderer_shutdown failed.\n");
    return 1;
  }
  return 0;
}

/* ------------------------------------------------------------------ */
/* Match helpers                                                       */
/* ------------------------------------------------------------------ */

static void game_start_match(Game *game) {
  arena_init(&game->arena, DEFAULT_ARENA_DIFFICULTY);
  player_init(&game->player1,
              arena_get_player1_spawn(&game->arena),
              PLAYER1_INITIAL_ANGLE, PLAYER1_COLOR);
  player_init(&game->player2,
              arena_get_player2_spawn(&game->arena),
              PLAYER2_INITIAL_ANGLE, PLAYER2_COLOR);
  game->frame_counter = 0;
}

/* Applies a state transition, performing the required setup for each target
 * state.  Transitioning to PLAYING from PAUSED does not reinit the match. */
static void game_apply_transition(Game *game, GameState next) {
  if (next == game->state) return;

  switch (next) {
    case GAME_STATE_PLAYING:
      /* Reinit only when coming from a non-playing state. */
      if (game->state != GAME_STATE_PAUSED) {
        game_start_match(game);
      }
      break;
    case GAME_STATE_MENU:
      menu_state_init(&game->menu);
      break;
    case GAME_STATE_GAME_OVER:
      /* winner is set on game->game_over.winner before calling this. */
      game_over_state_init(&game->game_over, game->game_over.winner);
      break;
    default:
      break;
  }

  game->state = next;
}

/* ------------------------------------------------------------------ */
/* Main event loop — subscribes once, runs until EXIT                 */
/* ------------------------------------------------------------------ */

static int game_run(Game *game) {
  if (game == NULL) return 1;

  uint8_t timer_bit_no;
  uint8_t keyboard_bit_no;

  if (timer_subscribe_int(&timer_bit_no) != 0) {
    printf("Failed to subscribe timer interrupts.\n");
    return 1;
  }

  if (keyboard_subscribe_int(&keyboard_bit_no) != 0) {
    printf("Failed to subscribe keyboard interrupts.\n");
    timer_unsubscribe_int();
    return 1;
  }

  int ipc_status;
  message msg;
  int result = 0;

  /* Render the initial menu frame before the loop starts. */
  if (menu_state_render(&game->menu) != 0) {
    printf("Initial menu render failed.\n");
    result = 1;
    game->state = GAME_STATE_EXIT;
  }

  while (game->state != GAME_STATE_EXIT) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("driver_receive failed.\n");
      result = 1;
      break;
    }

    if (!is_ipc_notify(ipc_status)) continue;
    if (_ENDPOINT_P(msg.m_source) != HARDWARE) continue;

    /* Keyboard interrupt: update raw key state. */
    if (msg.m_notify.interrupts & BIT(keyboard_bit_no)) {
      kbc_ih();
      if (!keyboard_has_error()) {
        keyboard_input_update(&game->keyboard, keyboard_get_scancode());
      }
    }

    /* Timer interrupt: drive the state machine. */
    if (msg.m_notify.interrupts & BIT(timer_bit_no)) {
      timer_int_handler();
      game->frame_counter++;
      game_tick(game);
    }
  }

  if (keyboard_unsubscribe_int() != 0) {
    printf("Failed to unsubscribe keyboard interrupts.\n");
    result = 1;
  }

  if (timer_unsubscribe_int() != 0) {
    printf("Failed to unsubscribe timer interrupts.\n");
    result = 1;
  }

  return result;
}

/* ------------------------------------------------------------------ */
/* Game tick — one call per timer interrupt                           */
/* ------------------------------------------------------------------ */

static void game_tick(Game *game) {
  /* Build actions from the current keyboard snapshot. */
  game_input_actions_from_keyboard(&game->actions, &game->keyboard);

  /* Clear one-shot events so they don't bleed into the next tick. */
  keyboard_input_clear_oneshots(&game->keyboard);

  switch (game->state) {
    case GAME_STATE_MENU:      state_menu_tick(game);      break;
    case GAME_STATE_PLAYING:   state_playing_tick(game);   break;
    case GAME_STATE_PAUSED:    state_paused_tick(game);    break;
    case GAME_STATE_GAME_OVER: state_game_over_tick(game); break;
    case GAME_STATE_EXIT:      break;
  }
}

/* ------------------------------------------------------------------ */
/* Per-state tick functions                                            */
/* ------------------------------------------------------------------ */

static void state_menu_tick(Game *game) {
  GameState next = game->state;
  menu_state_update(&game->menu, &game->actions, &next);
  game_apply_transition(game, next);

  /* Only render if we're still in the menu (i.e. no transition fired). */
  if (game->state == GAME_STATE_MENU) {
    if (menu_state_render(&game->menu) != 0) {
      printf("menu_state_render failed.\n");
      game->state = GAME_STATE_EXIT;
    }
  }
}

static void state_playing_tick(Game *game) {
  /* Pause takes priority over everything else. */
  if (game->actions.pause_requested) {
    game_apply_transition(game, GAME_STATE_PAUSED);
    return;
  }

  /* Player 1: move forward if W is held, otherwise rotate (placeholder). */
  if (game->actions.player1.move_forward) {
    Position next_pos = player_get_forward_position(&game->player1, PLAYER_MOVE_SPEED);
    if (!collision_player_walls(&game->arena, &game->player1, next_pos)) {
      player_set_position(&game->player1, next_pos);
    }
  } else {
    player_rotate(&game->player1, PLAYER_ROTATION_STEP);
  }

  /* Player 2: auto-rotate placeholder until mouse/second player is wired. */
  player_rotate(&game->player2, PLAYER_ROTATION_STEP);

  /* Check win conditions. */
  if (!player_is_alive(&game->player1)) {
    game->game_over.winner = 2;
    game_apply_transition(game, GAME_STATE_GAME_OVER);
    return;
  }
  if (!player_is_alive(&game->player2)) {
    game->game_over.winner = 1;
    game_apply_transition(game, GAME_STATE_GAME_OVER);
    return;
  }

  if (render_playing(game) != 0) {
    printf("render_playing failed.\n");
    game->state = GAME_STATE_EXIT;
  }
}

static void state_paused_tick(Game *game) {
  if (game->actions.pause_requested) {
    /* P again resumes the match without reinitializing. */
    game_apply_transition(game, GAME_STATE_PLAYING);
    return;
  }

  if (game->actions.back) {
    /* ESC from pause returns to main menu. */
    game_apply_transition(game, GAME_STATE_MENU);
    return;
  }

  /* Simulation is frozen — just redraw the static frame. */
  if (render_paused(game) != 0) {
    printf("render_paused failed.\n");
    game->state = GAME_STATE_EXIT;
  }
}

static void state_game_over_tick(Game *game) {
  GameState next = game->state;
  game_over_state_update(&game->game_over, &game->actions, &next);
  game_apply_transition(game, next);

  if (game->state == GAME_STATE_GAME_OVER) {
    if (game_over_state_render(&game->game_over) != 0) {
      printf("game_over_state_render failed.\n");
      game->state = GAME_STATE_EXIT;
    }
  }
}

/* ------------------------------------------------------------------ */
/* Render helpers                                                      */
/* ------------------------------------------------------------------ */

static int render_playing(const Game *game) {
  if (renderer_clear(PROJECT_BG_COLOR) != 0) return 1;
  if (arena_draw(&game->arena) != 0) return 1;
  if (player_draw(&game->player1) != 0) return 1;
  if (player_draw(&game->player2) != 0) return 1;
  return renderer_present();
}

static int render_paused(const Game *game) {
  if (renderer_clear(PROJECT_BG_COLOR) != 0) return 1;
  if (arena_draw(&game->arena) != 0) return 1;
  if (player_draw(&game->player1) != 0) return 1;
  if (player_draw(&game->player2) != 0) return 1;
  /* Yellow bar at the top of the arena signals pause. */
  if (renderer_draw_rectangle(0, 0, ARENA_PIXEL_WIDTH, PAUSE_BAR_HEIGHT, PAUSE_BAR_COLOR) != 0) {
    return 1;
  }
  return renderer_present();
}
