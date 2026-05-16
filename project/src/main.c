#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdio.h>

#include "renderer.h"

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

  if (renderer_clear(0x101010) != 0) {
    printf("Failed to clear the test background.\n");
    renderer_shutdown();
    return 1;
  }

  if (renderer_draw_rectangle(80, 60, 220, 140, 0x0033CC) != 0) {
    printf("Failed to draw the first test rectangle.\n");
    renderer_shutdown();
    return 1;
  }

  if (renderer_draw_rectangle(420, 240, 320, 180, 0xCC5500) != 0) {
    printf("Failed to draw the second test rectangle.\n");
    renderer_shutdown();
    return 1;
  }

  if (renderer_draw_rectangle(180, 500, 500, 80, 0x33AA33) != 0) {
    printf("Failed to draw the third test rectangle.\n");
    renderer_shutdown();
    return 1;
  }

  tickdelay(micros_to_ticks(1000000));

  if (renderer_shutdown() != 0) {
    printf("Failed to return to text mode.\n");
    return 1;
  }

  printf("Returned to text mode.\n");

  return 0;
}
