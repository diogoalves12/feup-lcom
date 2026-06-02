#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "arena.h"
#include "bullet.h"
#include "collision.h"
#include "game_input.h"
#include "game_over.h"
#include "game_state.h"
#include "pause_menu.h"
#include "i8042.h"
#include "keyboard.h"
#include "keyboard_input.h"
#include "menu.h"
#include "mouse.h"
#include "mouse_input.h"
#include "player.h"
#include "renderer.h"

#define PROJECT_VIDEO_MODE 0x115
#define PROJECT_BG_COLOR   0x101010

#define PAUSE_BAR_COLOR   0xFFFF00
#define PAUSE_BAR_HEIGHT  4
#define ARENA_PIXEL_WIDTH (ARENA_COLS * TILE_SIZE)

typedef struct {
  GameState        state;
  uint32_t         frame_counter;
  KeyboardInput    keyboard;
  MouseInput       mouse;
  uint8_t          mouse_packet[3];
  uint8_t          mouse_packet_idx;
  GameInputActions actions;
  Arena            arena;
  Player           player1;
  Player           player2;
  BulletSystem     bullets;
  MenuState        menu;
  PauseMenuState   pause_menu;
  GameOverState    game_over;
  bool             restart_requested;
} Game;

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

static int game_init(Game *game) {
  if (game == NULL) return 1;

  if (renderer_init(PROJECT_VIDEO_MODE) != 0) {
    printf("renderer_init failed for mode 0x%03X.\n", PROJECT_VIDEO_MODE);
    return 1;
  }

  keyboard_input_init(&game->keyboard);
  mouse_input_init(&game->mouse);
  game->mouse_packet_idx   = 0;
  game_input_actions_init(&game->actions);
  game->frame_counter       = 0;
  game->restart_requested   = false;
  game->game_over.winner    = 0;
  game->game_over.selection = GAME_OVER_SEL_RESTART;

  pause_menu_state_init(&game->pause_menu);
  bullet_system_init(&game->bullets);

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

static void game_start_match(Game *game) {
  arena_init(&game->arena, DEFAULT_ARENA_DIFFICULTY);
  player_init(&game->player1,
              arena_get_player1_spawn(&game->arena),
              PLAYER1_INITIAL_ANGLE, PLAYER1_COLOR);
  player_init(&game->player2,
              arena_get_player2_spawn(&game->arena),
              PLAYER2_INITIAL_ANGLE, PLAYER2_COLOR);
  bullet_system_reset(&game->bullets);
  game->frame_counter = 0;
}

static void game_apply_transition(Game *game, GameState next) {
  if (next == game->state) return;

  switch (next) {
    case GAME_STATE_PLAYING:
      if (game->restart_requested || game->state != GAME_STATE_PAUSED) {
        game_start_match(game);
        game->restart_requested = false;
      }
      break;
    case GAME_STATE_PAUSED:
      pause_menu_state_init(&game->pause_menu);
      break;
    case GAME_STATE_MENU:
      menu_state_init(&game->menu);
      break;
    case GAME_STATE_GAME_OVER:
      game_over_state_init(&game->game_over, game->game_over.winner);
      break;
    default:
      break;
  }

  game->state = next;
}

static int game_run(Game *game) {
  if (game == NULL) return 1;

  uint8_t timer_bit_no;
  uint8_t keyboard_bit_no;
  uint8_t mouse_bit_no;

  if (timer_subscribe_int(&timer_bit_no) != 0) {
    printf("Failed to subscribe timer interrupts.\n");
    return 1;
  }

  if (keyboard_subscribe_int(&keyboard_bit_no) != 0) {
    printf("Failed to subscribe keyboard interrupts.\n");
    timer_unsubscribe_int();
    return 1;
  }

  if (mouse_subscribe_int(&mouse_bit_no) != 0) {
    printf("Failed to subscribe mouse interrupts.\n");
    keyboard_unsubscribe_int();
    timer_unsubscribe_int();
    return 1;
  }

  if (mouse_enable_data_reporting_custom() != 0) {
    printf("Failed to enable mouse data reporting.\n");
    mouse_unsubscribe_int();
    keyboard_unsubscribe_int();
    timer_unsubscribe_int();
    return 1;
  }

  int ipc_status;
  message msg;
  int result = 0;

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

    if (msg.m_notify.interrupts & BIT(keyboard_bit_no)) {
      kbc_ih();
      if (!keyboard_has_error()) {
        keyboard_input_update(&game->keyboard, keyboard_get_scancode());
      }
    }

    if (msg.m_notify.interrupts & BIT(mouse_bit_no)) {
      mouse_ih();
      if (!mouse_get_error()) {
        uint8_t byte = mouse_get_byte();
        struct packet pkt;
        /* DEBUG: trace raw bytes and packet completion */
        printf("[mouse] byte: 0x%02X (idx=%d)\n", byte, game->mouse_packet_idx);
        if (mouse_sync_byte(byte, game->mouse_packet, &game->mouse_packet_idx)) {
          mouse_parse_packet_bytes(game->mouse_packet, &pkt);
          printf("[mouse] packet: lb=%d rb=%d mb=%d dx=%d dy=%d xov=%d yov=%d\n",
                 pkt.lb, pkt.rb, pkt.mb, pkt.delta_x, pkt.delta_y,
                 pkt.x_ov, pkt.y_ov);
          /* DEBUG: detect lb state changes */
          static bool prev_lb = false;
          if ((bool)pkt.lb != prev_lb) {
            printf("[mouse] lb changed: %d -> %d\n", prev_lb, pkt.lb);
            prev_lb = pkt.lb;
          }
          mouse_input_set(&game->mouse, pkt.lb, pkt.rb, pkt.mb);
        }
      }
    }

    if (msg.m_notify.interrupts & BIT(timer_bit_no)) {
      timer_int_handler();
      game->frame_counter++;
      game_tick(game);
    }
  }

  if (mouse_disable_data_reporting() != 0) {
    printf("Failed to disable mouse data reporting.\n");
    result = 1;
  }

  if (mouse_unsubscribe_int() != 0) {
    printf("Failed to unsubscribe mouse interrupts.\n");
    result = 1;
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

static void game_tick(Game *game) {
  game_input_actions_from_keyboard(&game->actions, &game->keyboard);
  game_input_actions_apply_mouse(&game->actions, &game->mouse);
  keyboard_input_clear_oneshots(&game->keyboard);

  /* DEBUG: detect player2 move_forward state changes */
  if (game->state == GAME_STATE_PLAYING) {
    static bool prev_p2_fwd = false;
    if (game->actions.player2.move_forward != prev_p2_fwd) {
      printf("[game_tick] player2 move_forward changed: %d -> %d  (mouse.move_forward=%d)\n",
             prev_p2_fwd, game->actions.player2.move_forward,
             game->mouse.move_forward);
      prev_p2_fwd = game->actions.player2.move_forward;
    }
  }

  switch (game->state) {
    case GAME_STATE_MENU:      state_menu_tick(game);      break;
    case GAME_STATE_PLAYING:   state_playing_tick(game);   break;
    case GAME_STATE_PAUSED:    state_paused_tick(game);    break;
    case GAME_STATE_GAME_OVER: state_game_over_tick(game); break;
    case GAME_STATE_EXIT:      break;
  }
}

static void state_menu_tick(Game *game) {
  GameState next = game->state;
  menu_state_update(&game->menu, &game->actions, &next);
  game_apply_transition(game, next);

  if (game->state == GAME_STATE_MENU) {
    if (menu_state_render(&game->menu) != 0) {
      printf("menu_state_render failed.\n");
      game->state = GAME_STATE_EXIT;
    }
  }
}

static void state_playing_tick(Game *game) {
  if (game->actions.pause_requested) {
    game_apply_transition(game, GAME_STATE_PAUSED);
    return;
  }

  if (game->actions.player1.move_forward) {
    Position next_pos = player_get_forward_position(&game->player1, PLAYER_MOVE_SPEED);
    if (!collision_player_walls(&game->arena, &game->player1, next_pos)) {
      player_set_position(&game->player1, next_pos);
    }
  } else {
    player_rotate(&game->player1, PLAYER_ROTATION_STEP);
  }

  if (game->actions.player1.shoot) {
    printf("[playing] p1 shoot=true\n");
    bullet_system_shoot(&game->bullets, &game->player1, 1);
  }

  if (game->actions.player2.move_forward) {
    Position next_pos = player_get_forward_position(&game->player2, PLAYER_MOVE_SPEED);
    if (!collision_player_walls(&game->arena, &game->player2, next_pos)) {
      player_set_position(&game->player2, next_pos);
    }
  } else {
    player_rotate(&game->player2, PLAYER_ROTATION_STEP);
  }

  if (game->actions.player2.shoot) {
    bullet_system_shoot(&game->bullets, &game->player2, 2);
  }

  bullet_system_update(&game->bullets, &game->arena, &game->player1, &game->player2);

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
  GameState next = game->state;
  bool was_confirm = game->actions.confirm;

  pause_menu_state_update(&game->pause_menu, &game->actions, &next);

  if (next != game->state) {
    if (next == GAME_STATE_PLAYING
        && game->pause_menu.selection == PAUSE_SEL_RESTART
        && was_confirm) {
      game->restart_requested = true;
    }
    game_apply_transition(game, next);
    return;
  }

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

static int render_playing(const Game *game) {
  if (renderer_clear(PROJECT_BG_COLOR) != 0) return 1;
  if (arena_draw(&game->arena) != 0) return 1;
  if (player_draw(&game->player1) != 0) return 1;
  if (player_draw(&game->player2) != 0) return 1;
  if (bullet_system_draw(&game->bullets) != 0) return 1;
  return renderer_present();
}

static int render_paused(const Game *game) {
  if (renderer_clear(PROJECT_BG_COLOR) != 0) return 1;
  if (arena_draw(&game->arena) != 0) return 1;
  if (player_draw(&game->player1) != 0) return 1;
  if (player_draw(&game->player2) != 0) return 1;
  if (bullet_system_draw(&game->bullets) != 0) return 1;
  if (renderer_draw_rectangle(0, 0, ARENA_PIXEL_WIDTH, PAUSE_BAR_HEIGHT, PAUSE_BAR_COLOR) != 0) {
    return 1;
  }
  if (pause_menu_state_render(&game->pause_menu) != 0) return 1;
  return renderer_present();
}
