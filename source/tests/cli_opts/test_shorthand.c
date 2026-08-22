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

  struct stark_cli_opts cli_opts = {
      .desc = "shorthand test", .optc = optc, .optv = optv};

#ifdef STARK_CLI_OPTS_ENABLE_ENV
  init_env(false);
#endif

  if (!stark_cli_opts_parse(&cli_opts, sizeof(argv) / sizeof(argv[0]), argv)) {
    return 2;
  }

  print_shorthand();
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  stark_cli_opts_free_token_pools(&cli_opts);
  stark_cli_opts_free_group_pools(&cli_opts);
#endif

  return 0;
}
