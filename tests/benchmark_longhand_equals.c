#include "test.h"

int main() {
  static const char *argv[] = {"benchmark-longhand-equals",
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

  const int argc = sizeof(argv) / sizeof(argv[0]);
  static struct mbx_opts app = {.desc = "benchmark-shorthand test"};

  if (!mbx_opts_init(&app, optc, opt)) {
    return 1;
  }

  for (int i = 0; i < 100000000; i++) {
    if (!mbx_opts_parse(&app, argc, argv)) {
      break;
    }
  }
}
