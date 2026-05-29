#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "arena.h"
#include "game_input.h"
#include "i8042.h"
#include "keyboard.h"
#include "keyboard_input.h"
#include "menu.h"
#include "player.h"
#include "renderer.h"

#define PROJECT_TEST_VIDEO_MODE 0x115
#define PROJECT_BACKGROUND_COLOR 0x101010
typedef struct {
  bool running;
  uint32_t frame_counter;
  KeyboardInput keyboard_input;
  GameInputActions input_actions;
  // Stores the generated map layout for the current match.
  Arena arena;
  Player player1;
  Player player2;
} Game;

static int game_init(Game *game);
static int game_loop(Game *game);
static void game_update(Game *game);
static int game_render(const Game *game);
static int game_shutdown(Game *game);
static bool game_player_collides_with_walls(const Arena *arena, const Player *player, Position position);
static bool game_can_move_player_to(const Game *game, const Player *player, Position position);

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");

  lcf_trace_calls("/home/lcom/labs/project/trace.txt");
  lcf_log_output("/home/lcom/labs/project/output.txt");

  if (lcf_start(argc, argv)) {
    return 1;
  }

  lcf_cleanup();

  return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  Game game;

  printf("LCOM project started.\n");

  if (game_init(&game) != 0) {
    return 1;
  }

  MenuResult menu_result = menu_loop();

  if (menu_result == MENU_RESULT_EXIT_GAME) {
    if (game_shutdown(&game) != 0) {
      printf("Failed to shut down the game cleanly.\n");
      return 1;
    }

    printf("Returned to text mode.\n");
    return 0;
  }

  printf("Starting game.\n");

  if (game_loop(&game) != 0) {
    game_shutdown(&game);
    return 1;
  }

  if (game_shutdown(&game) != 0) {
    printf("Failed to shut down the game cleanly.\n");
    return 1;
  }

  printf("Returned to text mode.\n");

  return 0;
}

static int game_init(Game *game) {
  if (game == NULL) {
    return 1;
  }

  if (renderer_init(PROJECT_TEST_VIDEO_MODE) != 0) {
    printf("Failed to initialize renderer for mode 0x%03X.\n", PROJECT_TEST_VIDEO_MODE);
    return 1;
  }

  if (arena_init(&game->arena, DEFAULT_ARENA_DIFFICULTY) != 0) {
    printf("Failed to initialize arena.\n");
    renderer_shutdown();
    return 1;
  }

  player_init(&game->player1,
              arena_get_player1_spawn(&game->arena),
              PLAYER1_INITIAL_ANGLE,
              PLAYER1_COLOR);
  player_init(&game->player2,
              arena_get_player2_spawn(&game->arena),
              PLAYER2_INITIAL_ANGLE,
              PLAYER2_COLOR);

  game->running = true;
  game->frame_counter = 0;
  keyboard_input_init(&game->keyboard_input);
  game_input_actions_init(&game->input_actions);

  return 0;
}

static int game_loop(Game *game) {
  if (game == NULL) {
    return 1;
  }

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

  while (game->running) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("driver_receive failed.\n");
      result = 1;
      break;
    }

    if (!is_ipc_notify(ipc_status)) {
      continue;
    }

    if (_ENDPOINT_P(msg.m_source) != HARDWARE) {
      continue;
    }

    if (msg.m_notify.interrupts & BIT(timer_bit_no)) {
      timer_int_handler();
      game_update(game);

      if (game_render(game) != 0) {
        printf("Failed to render a frame.\n");
        result = 1;
        break;
      }
    }

    if (msg.m_notify.interrupts & BIT(keyboard_bit_no)) {
      kbc_ih();

      if (!keyboard_has_error()) {
        keyboard_input_update(&game->keyboard_input, keyboard_get_scancode());
        game_input_actions_from_keyboard(&game->input_actions, &game->keyboard_input);

        if (game->input_actions.exit_requested) {
          game->running = false;
        }
      }
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

static void game_update(Game *game) {
  if (game == NULL) {
    return;
  }

  game->frame_counter++;

  if (game->input_actions.player1.move_forward) {
    Position next_position = player_get_forward_position(&game->player1, PLAYER_MOVE_SPEED);

    if (game_can_move_player_to(game, &game->player1, next_position)) {
      player_set_position(&game->player1, next_position);
    }
  } else {
    player_rotate(&game->player1, PLAYER_ROTATION_STEP);
  }

  player_rotate(&game->player2, PLAYER_ROTATION_STEP);
}

static bool game_player_collides_with_walls(const Arena *arena, const Player *player, Position position) {
  if (arena == NULL || player == NULL) {
    return true;
  }

  int left = position.x - player->width / 2;
  int right = position.x + player->width / 2 - 1;
  int top = position.y - player->height / 2;
  int bottom = position.y + player->height / 2 - 1;

  if (left < 0 || top < 0) {
    return true;
  }

  int left_col = left / TILE_SIZE;
  int right_col = right / TILE_SIZE;
  int top_row = top / TILE_SIZE;
  int bottom_row = bottom / TILE_SIZE;

  for (int row = top_row; row <= bottom_row; row++) {
    for (int col = left_col; col <= right_col; col++) {
      TileType type = arena_get_tile_type(arena, row, col);

      if (arena_is_wall_tile(type)) {
        return true;
      }
    }
  }

  return false;
}

static bool game_can_move_player_to(const Game *game, const Player *player, Position position) {
  if (game == NULL || player == NULL) {
    return false;
  }

  return !game_player_collides_with_walls(&game->arena, player, position);
}

static int game_render(const Game *game) {
  if (game == NULL) {
    return 1;
  }

  if (renderer_clear(PROJECT_BACKGROUND_COLOR) != 0) {
    return 1;
  }

  if (arena_draw(&game->arena) != 0) {
    return 1;
  }

  if (player_draw(&game->player1) != 0) {
    return 1;
  }

  if (player_draw(&game->player2) != 0) {
    return 1;
  }

  if (renderer_present() != 0) {
    return 1;
  }

  return 0;
}

static int game_shutdown(Game *game) {
  if (game != NULL) {
    game->running = false;
  }

  if (renderer_shutdown() != 0) {
    printf("Failed to return to text mode.\n");
    return 1;
  }

  return 0;
}
