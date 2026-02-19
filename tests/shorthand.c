#include "test.h"

int main() {
  // clang-format off
  const char *argv[] = {
    "./app/path",
    "-b",
    "-s", "a_str",
    "-i", "1",
    "-l", "1234567890",
    "-f", "1.234567",
    "-d", "1.234567",
    "-c"
  };
  // clang-format on

  struct cli_opts app;

  cli_opts_init(&app, opts, "An app description");
  cli_opts_parse(&app, ARRAY_LENGTH(argv), argv);
  test_print();
}
