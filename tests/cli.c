#include "test.h"

int main(const int argc, const char *argv[]) {
  struct mb_opts app = {.desc = "cli test"};

  if (!mb_opts_init(&app, opts, sizeof(opts[0]))) {
    return 1;
  }

  if (!mb_opts_parse(&app, argc, argv)) {
    return 1;
  }
  test_print();

  return 0;
}
