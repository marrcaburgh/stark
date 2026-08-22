#include "test.h"

int main(void) {
  struct stark_cli_opts cli_opts = {
      .desc = "benchmark-longhand-equals test", .optc = optc, .optv = optv};

#ifdef STARK_CLI_OPTS_ENABLE_ENV
  init_env(true);
#endif

  for (int i = 0; i < 10000000; i++) {
    // clang-format off
    char *argv[] = {"benchmark-longhand-equals",
                    "--r",
                    "--t",
                    "--y",
                    "--p=0",
                    "--a=1",
                    "--s=2",
                    "--h=0.0",
                    "--j=1.0",
                    "--k=2.0",
                    "--c='str0'",
                    "--v='str1'",
                    "--b='str2'"};
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
