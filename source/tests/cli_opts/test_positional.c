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

  struct stark_cli_opts cli_opts = {
      .desc = "positional test", .optc = optc, .optv = optv};

  if (!stark_cli_opts_init(&cli_opts)) {
    return 1;
  }

  if (!stark_cli_opts_init(&subcommand)) {
    return 2;
  }

  if (!stark_cli_opts_parse(&cli_opts, sizeof(argv) / sizeof(argv[0]), argv)) {
    return 3;
  }

  print_positional();
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  stark_cli_opts_free_token_pool(&cli_opts);
  stark_cli_opts_free_group_pools(&cli_opts);
#endif

  return 0;
}
