#include "stark/opts.h"
#include "test.h"

int main(void) {
  static char const *argv[] = {
    ""
  };
  static int const argc = sizeof(argv) / sizeof(argv[0]);
  struct stark_opts opts = {.optc = optc, .optv = optv, .desc = "benchmark-positional test"};

  if (!stark_opts_init(&opts)) {
    return 1;
  }

  for (int i = 0; i < 100000000; i++) {
    if (!stark_opts_parse(&opts, argc, argv)) {
      return 2;
    }
  }

  return 0;
}
