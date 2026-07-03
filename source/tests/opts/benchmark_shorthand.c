#include "test.h"

#include <stdbool.h>
#include <stddef.h>

int main(void) {
  struct stark_opts opts = {
      .desc = "benchmark-shorthand test", .optc = optc, .optv = optv};

  if (!stark_opts_init(&opts)) {
    return 1;
  }

#ifdef STARK_OPTS_ENABLE_ENV
  init_env(false);
#endif

  for (int i = 0; i < 100000000; i++) {
    char *argv[] = {"benchmark-shorthand",
                    "-q",
                    "-w",
                    "-e",
                    "-u",
                    "0",
                    "-i",
                    "1",
                    "-o",
                    "2",
                    "-d",
                    "0.0",
                    "-f",
                    "1.0",
                    "-g",
                    "2.0",
                    "-l",
                    "str0",
                    "-z",
                    "str1",
                    "-x",
                    "str2"};

    if (!stark_opts_parse(&opts, sizeof(argv) / sizeof(argv[0]), argv)) {
      return 2;
    }

#ifdef STARK_OPTS_ENABLE_HEAP
    stark_opts_free_token_pool(&opts);
#endif
  }

#ifdef STARK_OPTS_ENABLE_HEAP
  stark_opts_free_group_pools(&opts);
#endif

  return 0;
}
