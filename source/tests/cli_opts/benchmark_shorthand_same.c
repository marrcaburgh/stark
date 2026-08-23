#include "test.h"

int main(void) {
  struct stark_cli_opts cli_opts = {
      .desc = "benchmark-shorthand-same test", .optc = optc, .optv = optv};

#ifdef STARK_CLI_OPTS_ENABLE_ENV
  init_env(false);
#endif

  for (int i = 0; i < 10000000; i++) {
    // clang-format off
    char *argv[] = {"benchmark-shorthand-same",
                    "-q",
                    "-w",
                    "-e",
                    "-u0",
                    "-i1",
                    "-o2",
                    "-d0.0",
                    "-f1.0",
                    "-g2.0",
                    "-lstr0",
                    "-zstr1",
                    "-xstr2"};
    // clang-format on

    if (!stark_cli_opts_parse(&cli_opts, sizeof(argv) / sizeof(argv[0]),
                              argv)) {
      return 2;
    }

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
    stark_cli_opts_free_token_pools(&cli_opts);
#endif
  }

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  stark_cli_opts_free_group_pools(&cli_opts);
#endif

  return 0;
}
