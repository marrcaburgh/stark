#include "test.h"

int main() {
  static char const *argv[] = {"benchmark-longhand-equals",
                               "--r",
                               "--t",
                               "--y",
                               "--p=0",
                               "--a=1",
                               "--s=2",
                               "--h=0.0",
                               "--j=1.0",
                               "--k=2.0",
                               "--c='str0'",
                               "--v='str1'",
                               "--b='str2'"};

  int const argc = sizeof(argv) / sizeof(argv[0]);
  struct mbx_opts opts = {
      .desc = "benchmark-shorthand test", .optc = optc, .optv = optv};

  if (!mbx_opts_init(&opts)) {
    return 1;
  }

  for (int i = 0; i < 100000000; i++) {
    if (!mbx_opts_parse(&opts, argc, argv)) {
      return 2;
    }
  }

  return 0;
}
