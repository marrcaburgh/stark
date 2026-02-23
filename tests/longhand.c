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

  struct mbx_opts app = {.desc = "longhand test"};

  if (!mbx_opts_init(&app, opts, sizeof(opts) / sizeof(opts[0]))) {
    return 1;
  }

  if (!mbx_opts_parse(&app, ARRAY_LENGTH(argv), argv)) {
    return 2;
  }

  test_print();

  return 0;
}
