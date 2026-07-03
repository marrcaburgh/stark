#include "test.h"

int main(void) {
  // clang-format off
  char *argv[] = {
    "test-array",
    "--ba",
    "--ba",
    "--ba",
    "--ba",
    "--ba",
    "--ba",
    "--i64a", "1,2,3",
    "--i64a=4,5,6",
    "--f64a", "1.0,2.0,3.0",
    "--f64a=4.0,5.0,6.0",
    "--stra", (char[]){"String1\\,String1.5,String2,String3"},
    (char[]){"--stra=String4\\,String4.5,String5,String6"},
  };
  // clang-format on

  struct stark_opts opts = {.desc = "array test", .optc = optc, .optv = optv};

  if (!stark_opts_init(&opts)) {
    return 1;
  }

#ifdef STARK_OPTS_ENABLE_ENV
  init_env(true);
#endif

  if (!stark_opts_parse(&opts, sizeof(argv) / sizeof(argv[0]), argv)) {
    return 2;
  }

  print_array();
#ifdef STARK_OPTS_ENABLE_HEAP
  stark_opts_free_token_pool(&opts);
  stark_opts_free_group_pools(&opts);
#endif

  return 0;
}
