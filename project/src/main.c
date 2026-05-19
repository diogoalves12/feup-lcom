#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdio.h>

#include "renderer.h"
#include "game.h"

#define PROJECT_TEST_VIDEO_MODE 0x115

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

  printf("LCOM project started.\n");

  if (renderer_init(PROJECT_TEST_VIDEO_MODE) != 0) {
    printf("Failed to initialize renderer for mode 0x%03X.\n", PROJECT_TEST_VIDEO_MODE);
    return 1;
  }

  Player player = {
    .box = {.x = 100, .y = 100, .width = 30, .height = 30},
    .vx = 20,
    .vy = 0,
    .color = 0x0033CC
  };

  Wall walls[1] = {
    { .box = {.x = 115, .y = 80, .width = 40, .height = 100}, .color = 0x808080 }
  };

  renderer_clear(0x101010);
  renderer_draw_rectangle(walls[0].box.x, walls[0].box.y, walls[0].box.width, walls[0].box.height, walls[0].color);

  game_move_player(&player, walls, 1);

  renderer_draw_rectangle(player.box.x, player.box.y, player.box.width, player.box.height, player.color);

  tickdelay(micros_to_ticks(5000000));

  renderer_shutdown();
  return 0;
}
