#include "test.h"

int main() {
  static char const *argv[] = {"benchmark-mixed",
                               "-q",
                               "-w",
                               "-e",
                               "--r",
                               "--t",
                               "--y",
                               "-u0",
                               "-i1",
                               "-o2",
                               "--p=3",
                               "--a=4",
                               "--s=5",
                               "-d0.0",
                               "-f1.0",
                               "-g2.0",
                               "--h=3.0",
                               "--j=4.0",
                               "--k=5.0",
                               "-lstr0",
                               "-zstr1",
                               "-xstr2",
                               "--c=str3",
                               "--v=str4",
                               "--b=str5"};

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
