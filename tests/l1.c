#include "mb_clite_opts.h"
#include <stddef.h>
#include <string.h>

int main() {
  bool q, w, e, r, t, y;
  long u, i, o, p, a, s;
  double d, f, g, h, j, k;
  const char *l, *z, *x, *c, *v, *b;

  struct mb_opt opts[] = {
      // clang-format off
      MB_OPT_LIST(MB_OPT('q', NULL, MB_OPT_BOOL, &q, NULL),
                  MB_OPT('w', NULL, MB_OPT_BOOL, &w, NULL),
                  MB_OPT('e', NULL, MB_OPT_BOOL, &e, NULL),
                  MB_OPT('\0', "r", MB_OPT_BOOL, &r, NULL),
                  MB_OPT('\0', "t", MB_OPT_BOOL, &t, NULL),
                  MB_OPT('\0', "y", MB_OPT_BOOL, &y, NULL),

                  MB_OPT('u', NULL, MB_OPT_LONG, &u, NULL),
                  MB_OPT('i', NULL, MB_OPT_LONG, &i, NULL),
                  MB_OPT('o', NULL, MB_OPT_LONG, &o, NULL),
                  MB_OPT('\0', "p", MB_OPT_LONG, &p, NULL),
                  MB_OPT('\0', "a", MB_OPT_LONG, &a, NULL),
                  MB_OPT('\0', "s", MB_OPT_LONG, &s, NULL),

                  MB_OPT('d', NULL, MB_OPT_DBL, &d, NULL),
                  MB_OPT('f', NULL, MB_OPT_DBL, &f, NULL),
                  MB_OPT('g', NULL, MB_OPT_DBL, &g, NULL),
                  MB_OPT('\0', "h", MB_OPT_DBL, &h, NULL),
                  MB_OPT('\0', "j", MB_OPT_DBL, &j, NULL),
                  MB_OPT('\0', "k", MB_OPT_DBL, &k, NULL),

                  MB_OPT('l', NULL, MB_OPT_STR, &l, NULL),
                  MB_OPT('z', NULL, MB_OPT_STR, &z, NULL),
                  MB_OPT('x', NULL, MB_OPT_STR, &x, NULL),
                  MB_OPT('\0', "c", MB_OPT_STR, &c, NULL),
                  MB_OPT('\0', "v", MB_OPT_STR, &v, NULL),
                  MB_OPT('\0', "b", MB_OPT_STR, &b, NULL))
      // clang-format on
  };

  // static const char *argv[] = {
  //     "",       "-q",      "-w",       "-e",       "--r",
  //     "--t",    "--y",     "-u0",      "-i1",      "-o2",
  //     "--p=3",  "--a=4",   "--s=5",    "-d0.0",    "-f1.0",
  //     "-g2.0",  "--h=3.0", "--j=4.0",  "--k=5.0",  "-lstr0",
  //     "-zstr1", "-xstr2",  "--c=str3", "--v=str4", "--b=str5"};
  const char *argv[] = {"", "-q", "-q", "-q", "-q", "-q", "-q"};
  const int argc = sizeof(argv) / sizeof(*argv);

  struct mb_opts app = {.opts = opts, .desc = "l1 test"};

  if (!mb_opts_init(&app)) {
    return 1;
  }

  if (!mb_opts_init(&app)) {
    return 1;
  }

  for (int i = 0; i < 1000000; i++) {
    mb_opts_parse(&app, argc, argv);
  }

  return 0;
}
