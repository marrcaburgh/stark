#include "test.h"

int main(void) {
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

  struct mbx_opts opts = {.desc = "shorthand test", .optc = optc, .optv = optv};

  if (!mbx_opts_init(&opts)) {
    return 1;
  }

  if (!mbx_opts_parse(&opts, ARRAY_LENGTH(argv), argv)) {
    return 2;
  }

  print_shorthand();

  return 0;
}
