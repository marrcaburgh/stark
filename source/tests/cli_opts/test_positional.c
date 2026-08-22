#include "test.h"

#include <stdbool.h>

int main(void) {
  // clang-format off
  char *argv[] = {
      (char[]){"test-positional"},
      (char[]){"--subcommand"},
      (char[]){"3243232"},
      (char[]){"3.3"},
      (char[]){"Hello, World!"},
  };
  // clang-format on

  struct stark_cli_opts cli_opts = {
      .desc = "positional test", .optc = optc, .optv = optv};

  if (!stark_cli_opts_parse(&cli_opts, sizeof(argv) / sizeof(argv[0]), argv)) {
    return 3;
  }

  print_positional();
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  stark_cli_opts_free_token_pools(&cli_opts);
  stark_cli_opts_free_group_pools(&cli_opts);
#endif

  return 0;
}
