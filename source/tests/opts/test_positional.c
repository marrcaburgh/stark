#include "test.h"

#include <stdbool.h>

int main(void) {
  // clang-format off
  char *argv[] = {
      "test-positional",
      "--subcommand",
      "3243232",
      "3.3",
      "Hello, World!",
  };
  // clang-format on

  struct stark_opts opts = {
      .desc = "positional test", .optc = optc, .optv = optv};

  if (!stark_opts_init(&opts)) {
    return 1;
  }

  if (!stark_opts_init(&subcommand)) {
    return 2;
  }

  if (!stark_opts_parse(&opts, sizeof(argv) / sizeof(argv[0]), argv)) {
    return 3;
  }

  print_positional();
#ifdef STARK_OPTS_ENABLE_HEAP
  stark_opts_free_token_pool(&opts);
  stark_opts_free_group_pools(&opts);
#endif

  return 0;
}
