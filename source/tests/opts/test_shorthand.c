#include "test.h"

int main(void) {
  // clang-format off
  char *argv[] = {
    "test-shorthand",
    "-q",
    "-u1",
    "-i", "2",
    "-d1.0",
    "-f", "2.0",
    "-lString1",
    "-z", "String2"
  };
  // clang-format on

  struct stark_opts opts = {
      .desc = "shorthand test", .optc = optc, .optv = optv};

#ifdef STARK_OPTS_ENABLE_ENV
  init_env(false);
#endif

  if (!stark_opts_init(&opts)) {
    return 1;
  }

  if (!stark_opts_parse(&opts, sizeof(argv) / sizeof(argv[0]), argv)) {
    return 2;
  }

  print_shorthand();
#ifdef STARK_OPTS_ENABLE_HEAP
  stark_opts_free_token_pool(&opts);
  stark_opts_free_group_pools(&opts);
#endif

  return 0;
}
