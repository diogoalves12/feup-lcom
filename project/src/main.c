#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "arena.h"
#include "i8042.h"
#include "keyboard.h"
#include "renderer.h"
#include "menu.h"

#define PROJECT_TEST_VIDEO_MODE 0x115
#define PROJECT_BACKGROUND_COLOR 0x101010
#define PROJECT_ARENA_SEED 12345

typedef struct {
  bool running;
  uint32_t frame_counter;
  // Stores the generated map layout for the current match.
  Arena arena;
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

  // The seed is fixed for now, but later it can come from RTC or menu settings.
  if (arena_init(&game->arena, DEFAULT_ARENA_DIFFICULTY, PROJECT_ARENA_SEED) != 0) {
    printf("Failed to initialize arena.\n");
    renderer_shutdown();
    return 1;
  }

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

      if (!keyboard_has_error() && keyboard_get_scancode() == ESC_BREAKCODE) {
        game->running = false;
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
