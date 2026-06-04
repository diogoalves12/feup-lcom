#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "arena.h"
#include "arena_draw.h"
#include "collision.h"
#include "combat.h"
#include "config.h"
#include "game.h"
#include "game_input.h"
#include "game_over.h"
#include "game_state.h"
#include "i8042.h"
#include "keyboard.h"
#include "keyboard_input.h"
#include "menu.h"
#include "mouse.h"
#include "mouse_input.h"
#include "pause_menu.h"
#include "player.h"
#include "player_view.h"
#include "renderer.h"

#define ARENA_PIXEL_WIDTH   (ARENA_COLS * TILE_SIZE)

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
  CombatState      combat;
  bool             prev_p1_shoot;
  bool             prev_p2_shoot;
  MenuState        menu;
  PauseMenuState   pause_menu;
  GameOverState    game_over;
  bool             restart_requested;
} Game;

static int  game_setup(Game *game);
static int  game_loop(Game *game);
static int  game_shutdown(Game *game);

static int  game_subscribe_devices(uint8_t *timer_bit_no, uint8_t *keyboard_bit_no, uint8_t *mouse_bit_no);
static int  game_unsubscribe_devices(void);
static int  game_enable_mouse(void);
static int  game_disable_mouse(void);
static void game_handle_timer_interrupt(Game *game);
static void game_handle_keyboard_interrupt(Game *game);
static void game_handle_mouse_interrupt(Game *game);

static void game_start_match(Game *game);
static void game_apply_transition(Game *game, GameState next);
static void game_read_actions(Game *game);
static void game_update(Game *game);
static void game_render(Game *game);
static void game_tick(Game *game);

static void state_menu_update(Game *game);
static void state_playing_update(Game *game);
static void state_paused_update(Game *game);
static void state_game_over_update(Game *game);

static int  render_playing(const Game *game);
static int  render_paused(const Game *game);

static void update_player_movement(Player *player, const PlayerInputActions *input, const Arena *arena) {
  if (input->move_forward) {
    Position next_position = player_get_forward_position(player, PLAYER_MOVE_SPEED);
    if (!collision_player_walls(arena, player, next_position))
      player_set_position(player, next_position);
  } else {
    player_rotate(player, PLAYER_ROTATION_STEP);
  }
}

static void try_player_shot(Game *game, int player_num, Player *shooter, Player *target, bool action_shoot, bool *prev_shoot) {
  if (action_shoot && !*prev_shoot) {
    combat_try_shoot(&game->combat, player_num, shooter, target, &game->arena, game->frame_counter);
  }
  *prev_shoot = action_shoot;
}

static bool update_game_over_if_needed(Game *game) {
  if (!player_is_alive(&game->player1)) {
    game->game_over.winner = 2;
    game_apply_transition(game, GAME_STATE_GAME_OVER);
    return true;
  }
  if (!player_is_alive(&game->player2)) {
    game->game_over.winner = 1;
    game_apply_transition(game, GAME_STATE_GAME_OVER);
    return true;
  }
  return false;
}

static void render_hud(const Game *game) {
  player_view_draw_health_bar(&game->player1, 20, 20);
  player_view_draw_health_bar(&game->player2, ARENA_PIXEL_WIDTH - 20 - (PLAYER_DEFAULT_HEALTH * 15), 20);
}

static int game_setup(Game *game) {
  if (game == NULL) return 1;

  if (renderer_init(PROJECT_VIDEO_MODE) != 0) {
    printf("renderer_init failed for mode 0x%03X.\n", PROJECT_VIDEO_MODE);
    return 1;
  }

  keyboard_input_init(&game->keyboard);
  mouse_input_init(&game->mouse);
  game->mouse_packet_idx = 0;
  game_input_actions_init(&game->actions);
  game->frame_counter = 0;
  game->restart_requested = false;
  game->game_over.winner = 0;
  game->game_over.selection = GAME_OVER_SEL_RESTART;

  pause_menu_state_init(&game->pause_menu);
  combat_init(&game->combat);
  game->prev_p1_shoot = false;
  game->prev_p2_shoot = false;

  game->state = GAME_STATE_MENU;
  menu_state_init(&game->menu);
  if (menu_state_load_assets(&game->menu) != 0)
    printf("menu_state_load_assets failed, using fallback rendering.\n");

  return 0;
}

static int game_shutdown(Game *game) {
  menu_state_destroy_assets(&game->menu);
  if (renderer_shutdown() != 0) {
    printf("renderer_shutdown failed.\n");
    return 1;
  }
  return 0;
}

static void game_start_match(Game *game) {
  arena_init(&game->arena, DEFAULT_ARENA_DIFFICULTY);
  player_init(&game->player1, arena_get_player1_spawn(&game->arena), PLAYER1_INITIAL_ANGLE, PLAYER1_COLOR);
  player_init(&game->player2, arena_get_player2_spawn(&game->arena), PLAYER2_INITIAL_ANGLE, PLAYER2_COLOR);

  combat_reset(&game->combat);
  game->prev_p1_shoot = false;
  game->prev_p2_shoot = false;
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
      menu_state_reset(&game->menu);
      break;
    case GAME_STATE_GAME_OVER:
      game_over_state_init(&game->game_over, game->game_over.winner);
      break;
    default:
      break;
  }

  game->state = next;
}

static int game_subscribe_devices(uint8_t *timer_bit_no, uint8_t *keyboard_bit_no, uint8_t *mouse_bit_no) {
  if (timer_subscribe_int(timer_bit_no) != 0) {
    printf("Failed to subscribe timer interrupts.\n");
    return 1;
  }
  if (keyboard_subscribe_int(keyboard_bit_no) != 0) {
    printf("Failed to subscribe keyboard interrupts.\n");
    timer_unsubscribe_int();
    return 1;
  }
  if (mouse_subscribe_int(mouse_bit_no) != 0) {
    printf("Failed to subscribe mouse interrupts.\n");
    keyboard_unsubscribe_int();
    timer_unsubscribe_int();
    return 1;
  }
  return 0;
}

static int game_unsubscribe_devices(void) {
  int result = 0;
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

static int game_enable_mouse(void) {
  if (mouse_enable_data_reporting_custom() != 0) {
    printf("Failed to enable mouse data reporting.\n");
    return 1;
  }
  return 0;
}

static int game_disable_mouse(void) {
  if (mouse_disable_data_reporting() != 0) {
    printf("Failed to disable mouse data reporting.\n");
    return 1;
  }
  return 0;
}

static void game_handle_timer_interrupt(Game *game) {
  timer_int_handler();
  game->frame_counter++;
  game_tick(game);
}

static void game_handle_keyboard_interrupt(Game *game) {
  kbc_ih();
  if (!keyboard_has_error()) {
    keyboard_input_update(&game->keyboard, keyboard_get_scancode());
  }
}

static void game_handle_mouse_interrupt(Game *game) {
  mouse_ih();
  if (!mouse_get_error()) {
    uint8_t byte = mouse_get_byte();
    struct packet pkt;
    if (mouse_sync_byte(byte, game->mouse_packet, &game->mouse_packet_idx)) {
      mouse_parse_packet_bytes(game->mouse_packet, &pkt);
      mouse_input_set(&game->mouse, pkt.lb, pkt.rb, pkt.mb);
    }
  }
}

static int game_loop(Game *game) {
  if (game == NULL) return 1;

  uint8_t timer_bit_no;
  uint8_t keyboard_bit_no;
  uint8_t mouse_bit_no;

  if (game_subscribe_devices(&timer_bit_no, &keyboard_bit_no, &mouse_bit_no) != 0) return 1;

  if (game_enable_mouse() != 0) {
    game_unsubscribe_devices();
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

    if (msg.m_notify.interrupts & BIT(keyboard_bit_no))
      game_handle_keyboard_interrupt(game);

    if (msg.m_notify.interrupts & BIT(mouse_bit_no))
      game_handle_mouse_interrupt(game);

    if (msg.m_notify.interrupts & BIT(timer_bit_no))
      game_handle_timer_interrupt(game);
  }

  if (game_disable_mouse() != 0) result = 1;
  if (game_unsubscribe_devices() != 0) result = 1;

  return result;
}

static void game_read_actions(Game *game) {
  game_input_actions_from_keyboard(&game->actions, &game->keyboard);
  game_input_actions_apply_mouse(&game->actions, &game->mouse);
  keyboard_input_clear_oneshots(&game->keyboard);
}

static void game_update(Game *game) {
  switch (game->state) {
    case GAME_STATE_MENU:      state_menu_update(game);      break;
    case GAME_STATE_PLAYING:   state_playing_update(game);   break;
    case GAME_STATE_PAUSED:    state_paused_update(game);    break;
    case GAME_STATE_GAME_OVER: state_game_over_update(game); break;
    case GAME_STATE_EXIT:      break;
  }
}

static void game_render(Game *game) {
  int result = 0;
  switch (game->state) {
    case GAME_STATE_MENU:
      if (menu_state_render(&game->menu) != 0) {
        printf("menu_state_render failed.\n");
        result = 1;
      }
      break;
    case GAME_STATE_PLAYING:
      if (render_playing(game) != 0) {
        printf("render_playing failed.\n");
        result = 1;
      }
      break;
    case GAME_STATE_PAUSED:
      if (render_paused(game) != 0) {
        printf("render_paused failed.\n");
        result = 1;
      }
      break;
    case GAME_STATE_GAME_OVER:
      if (game_over_state_render(&game->game_over) != 0) {
        printf("game_over_state_render failed.\n");
        result = 1;
      }
      break;
    default: break;
  }
  if (result != 0) game->state = GAME_STATE_EXIT;
}

static void game_tick(Game *game) {
  game_read_actions(game);
  GameState state_before = game->state;
  game_update(game);
  if (game->state == state_before) game_render(game);
}

static void state_menu_update(Game *game) {
  GameState next = game->state;
  menu_state_update(&game->menu, &game->actions, &next);
  game_apply_transition(game, next);
}

static void state_playing_update(Game *game) {
  if (game->actions.pause_requested) {
    game_apply_transition(game, GAME_STATE_PAUSED);
    return;
  }

  update_player_movement(&game->player1, &game->actions.player1, &game->arena);
  try_player_shot(game, 1, &game->player1, &game->player2, game->actions.player1.shoot, &game->prev_p1_shoot);

  update_player_movement(&game->player2, &game->actions.player2, &game->arena);
  try_player_shot(game, 2, &game->player2, &game->player1, game->actions.player2.shoot, &game->prev_p2_shoot);

  update_game_over_if_needed(game);
}

static void state_paused_update(Game *game) {
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
  }
}

static void state_game_over_update(Game *game) {
  GameState next = game->state;
  game_over_state_update(&game->game_over, &game->actions, &next);
  game_apply_transition(game, next);
}

static int render_playing(const Game *game) {
  if (renderer_clear(PROJECT_BG_COLOR) != 0) return 1;
  if (arena_view_draw(&game->arena) != 0) return 1;
  if (player_view_draw(&game->player1) != 0) return 1;
  if (player_view_draw(&game->player2) != 0) return 1;
  render_hud(game);
  return renderer_present();
}

static int render_paused(const Game *game) {
  if (renderer_clear(PROJECT_BG_COLOR) != 0) return 1;
  if (arena_view_draw(&game->arena) != 0) return 1;
  if (player_view_draw(&game->player1) != 0) return 1;
  if (player_view_draw(&game->player2) != 0) return 1;
  render_hud(game);
  if (renderer_draw_rectangle(0, 0, ARENA_PIXEL_WIDTH, PAUSE_BAR_HEIGHT, PAUSE_BAR_COLOR) != 0) {
    return 1;
  }
  if (pause_menu_state_render(&game->pause_menu) != 0) return 1;
  return renderer_present();
}

int game_main_loop(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  Game game;

  if (game_setup(&game) != 0) return 1;

  int result = game_loop(&game);

  if (game_shutdown(&game) != 0) {
    printf("game_shutdown failed.\n");
    return 1;
  }

  return result;
}
