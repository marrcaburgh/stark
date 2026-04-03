#include "test.h"

int main(int const argc, char const **argv) {
  struct stark_opts opts = {.desc = "CLI test", .optc = optc, .optv = optv};

  if (!stark_opts_init(&opts)) {
    return 1;
  }

  if (!stark_opts_parse(&opts, argc, argv)) {
    return 2;
  }

  print_shorthand();
  print_longhand();

  return 0;
}
