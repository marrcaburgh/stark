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

  struct mb_opts app = {.opts = opts, .desc = "shorthand test"};

  if (!mb_opts_init(&app)) {
    return 1;
  }

  if (!mb_opts_parse(&app, ARRAY_LENGTH(argv), argv)) {
    return 2;
  }

  test_print();
}
