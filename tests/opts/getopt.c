#define _GNU_SOURCE
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char const *const restrict argv[] = {
    "benchmark_getopt",
    "-q", "-w", "-e",
    "--r", "--t", "--y",
    "-u0", "-i1", "-o2",
    "--p=3", "--a=4", "--s=5",
    "-d", "0.0", "-f", "1.0", "-g", "2.0",
    "--h", "3.0", "--j", "4.0", "--k", "5.0",
    "-lstr0", "-zstr1", "-xstr2",
    "--c=str3", "--v=str4", "--b=str5"
};
static int const argc = sizeof(argv) / sizeof(argv[0]);

int main(void) {
  static struct option longopts[] = {
        {"r", no_argument, 0, 'r'},
        {"t", no_argument, 0, 't'},
        {"y", no_argument, 0, 'y'},
        {"p", required_argument, 0, 'p'},
        {"a", required_argument, 0, 'a'},
        {"s", required_argument, 0, 's'},
        {"h", required_argument, 0, 'h'},
        {"j", required_argument, 0, 'j'},
        {"k", required_argument, 0, 'k'},
        {"c", required_argument, 0, 'c'},
        {"v", required_argument, 0, 'v'},
        {"b", required_argument, 0, 'b'},
        {0, 0, 0, 0}
  };

  for (int i = 0; i < 100000000; i++) {
    optind = 1;
    opterr = 0;

    while (getopt_long(argc, argv, "qweu:i:o:d:f:g:l:z:x", longopts, NULL) != -1)
      ;
  }

  return 0;
}
