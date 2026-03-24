#include "test.h"

#include <stdbool.h>
#include <stdio.h>

int main() {
  // clang-format off
  const char *argv[] = {
      "positional",
      "--subcommand",
      "-u", "3243232",
      "-d", "3.3",
      "-l", "Hello, World!",
  };
  // clang-format on

  static struct mbx_opts opts = {.desc = "positional test"};

  if (!mbx_opts_init(&opts, optc, optv)) {
    return 1;
  }

  if (!mbx_opts_init(&subcommand, optc, optv)) {
    return 2;
  }

  if (!mbx_opts_parse(&opts, ARRAY_LENGTH(argv), argv)) {
    return 3;
  }

  print_positional();

  return 0;
}
