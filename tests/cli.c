#include "test.h"

int main(const int argc, const char **argv) {
  mbx_opts opts = {.desc = "CLI test"};

  if (!mbx_opts_init(&opts, optc, optv)) {
    return 1;
  }

  if (!mbx_opts_parse(&opts, argc, argv)) {
    return 2;
  }

  print_shorthand();
  print_longhand();

  return 0;
}
