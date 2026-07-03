#include "test.h"

int main(void) {
  struct stark_opts opts = {
      .desc = "benchmark-mixed test", .optc = optc, .optv = optv};

  if (!stark_opts_init(&opts)) {
    return 1;
  }
#ifdef STARK_OPTS_ENABLE_ENV
  init_env(false);
  init_env(true);
#endif

  for (int i = 0; i < 100000000; i++) {
    // clang-format off
    char *argv[] = {"benchmark-mixed",
                    "-q",
                    "--p=3",
                    "-w",
                    "--h", "3.0",
                    "-e",
                    "--c=str3",
                    "--r",
                    "-u0",
                    "--t",
                    "-d", "0.0",
                    "--y",
                    "-lstr0",
                    "-i1",
                    "--a=4",
                    "-o2",
                    "-f", "1.0",
                    "-zstr1",
                    "--s=5",
                    "--j", "4.0",
                    "-xstr2",
                    "--k", "5.0",
                    "--v=str4",
                    "-g", "2.0",
                    "--b=str5"};
    // clang-format on

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
