#include <lcom/lcf.h>
#include <stdio.h>

#include "config.h"
#include "game.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/project/debug/trace.txt");
  lcf_log_output("/home/lcom/labs/project/debug/output.txt");
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
