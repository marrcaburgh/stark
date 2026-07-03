#include "stark/opts.h"
#include "test.h"

int main(int argc, char *argv[]) {
  struct stark_opts opts = {.desc = "CLI test", .optc = optc, .optv = optv};

#ifdef STARK_OPTS_ENABLE_ENV
  init_env(false);
  init_env(true);
#endif

  if (!stark_opts_init(&opts)) {
    return 1;
  }

  if (!stark_opts_init(&subcommand)) {
    return 2;
  }

  if (!stark_opts_parse(&opts, argc, argv)) {
    return 3;
  }

  print_shorthand();
  print_longhand();
  print_array();
#ifdef STARK_OPTS_ENABLE_HEAP
  stark_opts_free_token_pool(&opts);
  stark_opts_free_token_pool(&subcommand);
  stark_opts_free_group_pools(&opts);
  stark_opts_free_group_pools(&subcommand);
#endif

  return 0;
}
