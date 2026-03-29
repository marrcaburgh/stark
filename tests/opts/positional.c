#include "test.h"

#include <stdbool.h>
#include <stdio.h>

int main() {
  // clang-format off
  char const *argv[] = {
      "positional",
      "--subcommand",
      "3243232",
      "3.3",
      "Hello, World!",
  };
  // clang-format on

  struct mbx_opts opts = {
      .desc = "positional test", .optc = optc, .optv = optv};

  if (!mbx_opts_init(&opts)) {
    return 1;
  }

  if (!mbx_opts_init(&subcommand)) {
    return 2;
  }

  if (!mbx_opts_parse(&opts, ARRAY_LENGTH(argv), argv)) {
    return 3;
  }

  print_positional();

  return 0;
}
