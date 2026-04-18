#include "test.h"

#include <stdbool.h>
#include <stddef.h>

int main(void) {
  static char const *argv[] = {"benchmark-shorthand",
                               "-q",
                               "-w",
                               "-e",
                               "-u", "0",
                               "-i", "1",
                               "-o", "2",
                               "-d", "0.0",
                               "-f", "1.0",
                               "-g", "2.0",
                               "-l", "str0",
                               "-z", "str1",
                               "-x", "str2"};

  static int const argc = sizeof(argv) / sizeof(argv[0]);
  struct stark_opts opts = {
      .desc = "benchmark-shorthand test", .optc = optc, .optv = optv};

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
