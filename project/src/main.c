#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "arena.h"
#include "i8042.h"
#include "keyboard.h"
#include "renderer.h"
#include "game.h"
#include "menu.h"

#define PROJECT_TEST_VIDEO_MODE 0x115
#define PROJECT_BACKGROUND_COLOR 0x101010
#define PROJECT_ARENA_SEED 12345

typedef struct {
  bool running;
  uint32_t frame_counter;
  // Stores the generated map layout for the current match.
  Arena arena;
  
  Player player1;
  bool key_w_pressed;
  bool key_a_pressed;
  bool key_s_pressed;
  bool key_d_pressed;
} Game;

static int game_init(Game *game);
static int game_loop(Game *game);
static void game_update(Game *game);
static int game_render(const Game *game);
static int game_shutdown(Game *game);

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

  MenuState menu_result = menu_loop();

  if (menu_result == MENU_EXIT_GAME) {
    printf("Exiting from menu.\n");

    if (game_shutdown(&game) != 0) {
      printf("Failed to shut down the game cleanly.\n");
      return 1;
    }

    printf("Returned to text mode.\n");
    return 0;
  }

  if (menu_result == MENU_START_GAME) {
    printf("Starting game.\n");

    if (game_loop(&game) != 0) {
      game_shutdown(&game);
      return 1;
    }
  }

  if (game_shutdown(&game) != 0) {
    printf("Failed to shut down the game cleanly.\n");
    return 1;
  }

  printf("Returned to text mode.\n");

  renderer_shutdown();
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

  // The seed is fixed for now, but later it can come from RTC or menu settings.
  if (arena_init(&game->arena, DEFAULT_ARENA_DIFFICULTY, PROJECT_ARENA_SEED) != 0) {
    printf("Failed to initialize arena.\n");
    renderer_shutdown();
    return 1;
  }

  Position spawn = arena_get_player1_spawn(&game->arena);
  game->player1.box.x = spawn.x;
  game->player1.box.y = spawn.y;
  game->player1.box.width = 16;
  game->player1.box.height = 16;
  game->player1.vx = 0;
  game->player1.vy = 0;
  game->player1.color = 0x0033CC;

  game->key_w_pressed = false;
  game->key_a_pressed = false;
  game->key_s_pressed = false;
  game->key_d_pressed = false;

  game->running = true;
  game->frame_counter = 0;

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
        uint8_t scancode = keyboard_get_scancode();

        if (scancode == ESC_BREAKCODE) {
          game->running = false;
        } else if (scancode == 0x11) { // W make
          game->key_w_pressed = true;
        } else if (scancode == 0x91) { // W break
          game->key_w_pressed = false;
        } else if (scancode == 0x1E) { // A make
          game->key_a_pressed = true;
        } else if (scancode == 0x9E) { // A break
          game->key_a_pressed = false;
        } else if (scancode == 0x1F) { // S make
          game->key_s_pressed = true;
        } else if (scancode == 0x9F) { // S break
          game->key_s_pressed = false;
        } else if (scancode == 0x20) { // D make
          game->key_d_pressed = true;
        } else if (scancode == 0xA0) { // D break
          game->key_d_pressed = false;
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

  int16_t speed = 5;
  game->player1.vx = 0;
  game->player1.vy = 0;

  if (game->key_w_pressed) game->player1.vy -= speed;
  if (game->key_s_pressed) game->player1.vy += speed;
  if (game->key_a_pressed) game->player1.vx -= speed;
  if (game->key_d_pressed) game->player1.vx += speed;

  game_move_player(&game->player1, &game->arena);
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

  if (renderer_draw_rectangle(game->player1.box.x, game->player1.box.y, game->player1.box.width, game->player1.box.height, game->player1.color) != 0) {
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
