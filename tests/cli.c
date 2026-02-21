#include "test.h"

int main(const int argc, const char *argv[]) {
  struct mb_opts app = {.opts = opts, .desc = "cli test"};

  if (!mb_opts_init(&app)) {
    return 1;
  }

  if (!mb_opts_parse(&app, argc, argv)) {
    return 1;
  }
  test_print();

  return 0;
}
