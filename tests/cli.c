#include "test.h"

int main(const int argc, const char *argv[]) {
  struct cli_opts app;
  cli_opts_init(&app, opts, "An app description");

  if (!cli_opts_parse(&app, argc, argv)) {
    return 1;
  }
  test_print();

  return 0;
}
