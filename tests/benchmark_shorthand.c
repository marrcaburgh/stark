#include "test.h"

#include <stdbool.h>
#include <stddef.h>

int main() {
  static const char *argv[] = {"benchmark-shorthand",
                               "-q",
                               "-w",
                               "-e",
                               "-u0",
                               "-i1",
                               "-o2",
                               "-d0.0",
                               "-f1.0",
                               "-g2.0",
                               "-lstr0",
                               "-zstr1",
                               "-xstr2"};

  const int argc = sizeof(argv) / sizeof(argv[0]);
  static struct mbx_opts opts = {.desc = "benchmark-shorthand test"};

  if (!mbx_opts_init(&opts, optc, optv)) {
    return 1;
  }

  for (int i = 0; i < 100000000; i++) {
    if (!mbx_opts_parse(&opts, argc, argv)) {
      break;
    }
  }

  return 0;
}
