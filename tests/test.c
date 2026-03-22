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

struct mbx_opt optv[] = {
    MBX_OPT('q', NULL, NULL, MBX_OPT_TYPE_BOOL, &q, NULL, NULL, NULL),
    MBX_OPT('w', NULL, NULL, MBX_OPT_TYPE_BOOL, &w, NULL, NULL, NULL),
    MBX_OPT('e', NULL, NULL, MBX_OPT_TYPE_BOOL, &e, NULL, NULL, NULL),
    MBX_OPT('\0', "r", NULL, MBX_OPT_TYPE_BOOL, &r, NULL, NULL, NULL),
    MBX_OPT('\0', "t", NULL, MBX_OPT_TYPE_BOOL, &t, NULL, NULL, NULL),
    MBX_OPT('\0', "y", NULL, MBX_OPT_TYPE_BOOL, &y, NULL, NULL, NULL),

    MBX_OPT('u', NULL, NULL, MBX_OPT_TYPE_LONG, &u, NULL, NULL, NULL),
    MBX_OPT('i', NULL, NULL, MBX_OPT_TYPE_LONG, &i, NULL, NULL, NULL),
    MBX_OPT('o', NULL, NULL, MBX_OPT_TYPE_LONG, &o, NULL, NULL, NULL),
    MBX_OPT('\0', "p", NULL, MBX_OPT_TYPE_LONG, &p, NULL, NULL, NULL),
    MBX_OPT('\0', "a", NULL, MBX_OPT_TYPE_LONG, &a, NULL, NULL, NULL),
    MBX_OPT('\0', "s", NULL, MBX_OPT_TYPE_LONG, &s, NULL, NULL, NULL),

    MBX_OPT('d', NULL, NULL, MBX_OPT_TYPE_DBL, &d, NULL, NULL, NULL),
    MBX_OPT('f', NULL, NULL, MBX_OPT_TYPE_DBL, &f, NULL, NULL, NULL),
    MBX_OPT('g', NULL, NULL, MBX_OPT_TYPE_DBL, &g, NULL, NULL, NULL),
    MBX_OPT('\0', "h", NULL, MBX_OPT_TYPE_DBL, &h, NULL, NULL, NULL),
    MBX_OPT('\0', "j", NULL, MBX_OPT_TYPE_DBL, &j, NULL, NULL, NULL),
    MBX_OPT('\0', "k", NULL, MBX_OPT_TYPE_DBL, &k, NULL, NULL, NULL),

    MBX_OPT('l', NULL, NULL, MBX_OPT_TYPE_STR, &l, NULL, NULL, NULL),
    MBX_OPT('z', NULL, NULL, MBX_OPT_TYPE_STR, &z, NULL, NULL, NULL),
    MBX_OPT('x', NULL, NULL, MBX_OPT_TYPE_STR, &x, NULL, NULL, NULL),
    MBX_OPT('\0', "c", NULL, MBX_OPT_TYPE_STR, &c, NULL, NULL, NULL),
    MBX_OPT('\0', "v", NULL, MBX_OPT_TYPE_STR, &v, NULL, NULL, NULL),
    MBX_OPT('\0', "b", NULL, MBX_OPT_TYPE_STR, &b, NULL, NULL, NULL)
    // to keep the curly brace below, and opts on the left.
};

const int optc = ARRAY_LENGTH(optv);
