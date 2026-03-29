#include "test.h"

int main(const int argc, const char **argv) {
  struct mbx_opts opts = {.desc = "CLI test", .optc = optc, .optv = optv};

  if (!mbx_opts_init(&opts)) {
    return 1;
  }

  if (!mbx_opts_parse(&opts, argc, argv)) {
    return 2;
  }

  print_shorthand();
  print_longhand();

  return 0;
}
