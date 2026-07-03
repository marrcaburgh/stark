#include "stark/opts.h"
#include "test.h"

int main(void) {
  // clang-format off
  char *argv[] = {
    "test-longhand",
    "--r",
    "--p=1",
    "--a", "2",
    "--h=1.0",
    "--j", "2.0",
    "--c=String1",
    "--v", "String2"
  };
  // clang-format on

  struct stark_opts opts = {
      .desc = "longhand test", .optc = optc, .optv = optv};

  if (!stark_opts_init(&opts)) {
    return 1;
  }

#ifdef STARK_OPTS_ENABLE_ENV
  init_env(true);
#endif

  if (!stark_opts_parse(&opts, sizeof(argv) / sizeof(argv[0]), argv)) {
    return 2;
  }

  print_longhand();
#ifdef STARK_OPTS_ENABLE_HEAP
  stark_opts_free_token_pool(&opts);
  stark_opts_free_group_pools(&opts);
#endif

  return 0;
}
