#include "test.h"

int main(const int argc, const char *argv[]) {
  struct cli_opts app;
  cli_opts_init(&app, opts, "An app description");
  cli_opts_parse(&app, argc, argv);
  test_print();
}
