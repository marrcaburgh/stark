#include "test.h"

int main() {
  // clang-format off
  static const char *argv[] = {"benchmark-longhand",
                               "--r",
                               "--t",
                               "--y",
                               "--p", "0",
                               "--a", "1",
                               "--s", "2",
                               "--h", "0.0",
                               "--j", "1.0",
                               "--k", "2.0",
                               "--c", "'str0'",
                               "--v", "'str1'",
                               "--b", "'str2'"};
  // clang-format on

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
}
