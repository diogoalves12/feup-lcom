#include <lcom/lcf.h>
#include <stdio.h>

#include "video.h"

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

  if (video_map_vram(PROJECT_TEST_VIDEO_MODE) != 0) {
    printf("Failed to map VRAM for mode 0x%03X.\n", PROJECT_TEST_VIDEO_MODE);
    return 1;
  }

  if (video_set_mode(PROJECT_TEST_VIDEO_MODE) != 0) {
    printf("Failed to enter graphics mode 0x%03X.\n", PROJECT_TEST_VIDEO_MODE);
    return 1;
  }

  if (vg_exit() != 0) {
    printf("Failed to return to text mode.\n");
    return 1;
  }

  printf("Returned to text mode.\n");

  return 0;
}
