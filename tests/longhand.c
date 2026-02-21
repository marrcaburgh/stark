#include "test.h"

int main() {
  // clang-format off
  const char *argv[] = {
    "./app/path",
    "--boolean",
    "--string=a_str",
    "--integer", "1",
    "--long", "1234567890l",
    "--float", "1.234567",
    "--double", "1.234567",
    "--callback"
  };
  // clang-format on

  struct mb_opts app = {.opts = opts, .desc = "longhand test"};
  if (!mb_opts_init(&app)) {
    return 1;
  }

  if (!mb_opts_parse(&app, ARRAY_LENGTH(argv), argv)) {
    return 2;
  }

  test_print();

  return 0;
}
