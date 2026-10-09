#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_FOLDER "build/"

int main(int argc, char **argv) {

  NOB_GO_REBUILD_URSELF(argc, argv);
  if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, "cc");
  nob_cmd_append(&cmd, "-Wall", "-Wextra");
  nob_cmd_append(&cmd, "-ggdb");
  nob_cmd_append(&cmd, "-lm");
  // for some reason -O3 breaks everything
  // nob_cmd_append(&cmd, "-O3");
  nob_cmd_append(&cmd, "main.c");
  nob_cmd_append(&cmd, "-o" BUILD_FOLDER"ds4led");

  if (!nob_cmd_run(&cmd)) return 1;

  return 0;
}