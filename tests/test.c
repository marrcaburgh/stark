#include "test.h"

#include <stdio.h>

void callback(const void *const ctx) { printf("callback\n"); }

bool validate(const char *const str, const void *const ctx) { return true; }

void print_shorthand() {
  printf("shorthand:\n\tbool: %b\n\tlong1: %lu\n\tlong2: %lu\n\tdouble1: "
         "%f\n\tdouble2: %f\n\tconst char *1: %s\n\tconst char *2: %s\n",
         q, u, i, d, f, l, z);
}

void print_longhand() {
  printf("longhand:\n\tbool: %b\n\tlong1: %lu\n\tlong2: %lu\n\tdouble1: "
         "%f\n\tdouble2: %f\n\tconst char *1: %s\n\tconst char *2: %s\n",
         r, p, a, h, j, c, v);
}

bool q, w, e, r, t, y;
long u, i, o, p, a, s;
double d, f, g, h, j, k;
const char *l, *z, *x, *c, *v, *b;

struct mbx_opt opt[] = {
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

const int optc = ARRAY_LENGTH(opt);
