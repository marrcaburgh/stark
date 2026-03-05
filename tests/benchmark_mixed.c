#include "test.h"

int main() {
  static const char *argv[] = {"benchmark-mixed",
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
