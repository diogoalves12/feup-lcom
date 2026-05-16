#include <lcom/lcf.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");

  lcf_trace_calls("/home/lcom/project/trace.txt");
  lcf_log_output("/home/lcom/project/output.txt");

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

  return 0;
}
