#include "test.h"

int main(const int argc, const char *argv[]) {
  struct mbx_opts app = {.desc = "cli test"};

  if (!mbx_opts_init(&app, opts, sizeof(opts) / sizeof(opts[0]))) {
    return 1;
  }

  if (!mbx_opts_parse(&app, argc, argv)) {
    return 1;
  }
  test_print();

  return 0;
}
