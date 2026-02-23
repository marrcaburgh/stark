#include "mbx/opts.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int main() {
  bool q, w, e, r, t, y;
  long u, i, o, p, a, s;
  double d, f, g, h, j, k;
  const char *l, *z, *x, *c, *v, *b;

  struct mbx_opt opts[] = {
      MBX_OPT('q', NULL, MBX_OPT_TYPE_BOOL, &q, NULL),
      MBX_OPT('w', NULL, MBX_OPT_TYPE_BOOL, &w, NULL),
      MBX_OPT('e', NULL, MBX_OPT_TYPE_BOOL, &e, NULL),
      MBX_OPT('\0', "r", MBX_OPT_TYPE_BOOL, &r, NULL),
      MBX_OPT('\0', "t", MBX_OPT_TYPE_BOOL, &t, NULL),
      MBX_OPT('\0', "y", MBX_OPT_TYPE_BOOL, &y, NULL),

      MBX_OPT('u', NULL, MBX_OPT_TYPE_LONG, &u, NULL),
      MBX_OPT('i', NULL, MBX_OPT_TYPE_LONG, &i, NULL),
      MBX_OPT('o', NULL, MBX_OPT_TYPE_LONG, &o, NULL),
      MBX_OPT('\0', "p", MBX_OPT_TYPE_LONG, &p, NULL),
      MBX_OPT('\0', "a", MBX_OPT_TYPE_LONG, &a, NULL),
      MBX_OPT('\0', "s", MBX_OPT_TYPE_LONG, &s, NULL),

      MBX_OPT('d', NULL, MBX_OPT_TYPE_DBL, &d, NULL),
      MBX_OPT('f', NULL, MBX_OPT_TYPE_DBL, &f, NULL),
      MBX_OPT('g', NULL, MBX_OPT_TYPE_DBL, &g, NULL),
      MBX_OPT('\0', "h", MBX_OPT_TYPE_DBL, &h, NULL),
      MBX_OPT('\0', "j", MBX_OPT_TYPE_DBL, &j, NULL),
      MBX_OPT('\0', "k", MBX_OPT_TYPE_DBL, &k, NULL),

      MBX_OPT('l', NULL, MBX_OPT_TYPE_STR, &l, NULL),
      MBX_OPT('z', NULL, MBX_OPT_TYPE_STR, &z, NULL),
      MBX_OPT('x', NULL, MBX_OPT_TYPE_STR, &x, NULL),
      MBX_OPT('\0', "c", MBX_OPT_TYPE_STR, &c, NULL),
      MBX_OPT('\0', "v", MBX_OPT_TYPE_STR, &v, NULL),
      MBX_OPT('\0', "b", MBX_OPT_TYPE_STR, &b, NULL)
      // to keep the curly brace below, and opts on the left.
  };

  static const char *argv[] = {
      "l1-shorthand", "-q",    "-w",    "-e",     "-u0",    "-i1",   "-o2",
      "-d0.0",        "-f1.0", "-g2.0", "-lstr0", "-zstr1", "-xstr2"};

  // static const char *argv[] = {
  //     "l1-longhand", "--r",        "--t",     "--y",     "--p=0",
  //     "--a=1",       "--s=2",      "--h=0.0", "--j=1.0", "--k=2.0",
  //     "--c='str0'",  "--v='str1'", "--b=str2"};

  // static const char *argv[] = {
  //     "l1-mixed", "-q",      "-w",       "-e",       "--r",
  //     "--t",      "--y",     "-u0",      "-i1",      "-o2",
  //     "--p=3",    "--a=4",   "--s=5",    "-d0.0",    "-f1.0",
  //     "-g2.0",    "--h=3.0", "--j=4.0",  "--k=5.0",  "-lstr0",
  //     "-zstr1",   "-xstr2",  "--c=str3", "--v=str4", "--b=str5"};

  const int argc = sizeof(argv) / sizeof(argv[0]);
  struct mbx_opts app = {.desc = "l1 test"};

  printf("%s\n", argv[0]);

  if (!mbx_opts_init(&app, opts, sizeof(opts) / sizeof(opts[0]))) {
    return 1;
  }

  for (int i = 0; i < 100000000; i++) {
    mbx_opts_parse(&app, argc, argv);
  }

  return 0;
}
