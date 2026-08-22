#include "stark/cli_opts.h"
#include "test.h"

int main(int argc, char *argv[]) {
  struct stark_cli_opts cli_opts = {
      .desc = "CLI test", .optc = optc, .optv = optv};

#ifdef STARK_CLI_OPTS_ENABLE_ENV
  init_env(false);
  init_env(true);
#endif

  if (!stark_cli_opts_parse(&cli_opts, argc, argv)) {
    return 3;
  }

  print_shorthand();
  print_longhand();
  print_array();
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  stark_cli_opts_free_token_pools(&cli_opts);
  stark_cli_opts_free_group_pools(&cli_opts);
#endif

  return 0;
}
