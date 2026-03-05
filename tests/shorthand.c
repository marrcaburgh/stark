#include "test.h"

int main() {
  // clang-format off
  const char *argv[] = {
    "shorthand",
    "-q",
    "-u1",
    "-i", "2",
    "-d1.0",
    "-f", "2.0",
    "-lString1",
    "-z", "String2"
  };
  // clang-format on

  static struct mbx_opts app = {.desc = "shorthand test"};

  if (!mbx_opts_init(&app, optc, opt)) {
    return 1;
  }

  if (!mbx_opts_parse(&app, ARRAY_LENGTH(argv), argv)) {
    return 2;
  }

  print_shorthand();

  return 0;
}
