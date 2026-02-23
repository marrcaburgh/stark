#include "test.h"

int main() {
  // clang-format off
  const char *argv[] = {
    "./app/path",
    "-b",
    "-s", "a_str",
    "-i1",
    "-l", "1234567890",
    "-f", "1.234567",
    "-d1.234567",
    "-c"
  };
  // clang-format on

  struct mbx_opts app = {.desc = "shorthand test"};

  if (!mbx_opts_init(&app, opts, sizeof(opts) / sizeof(opts[0]))) {
    return 1;
  }

  if (!mbx_opts_parse(&app, ARRAY_LENGTH(argv), argv)) {
    return 2;
  }

  test_print();
}
