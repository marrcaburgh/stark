#include "test.h"

#include <stdio.h>

void callback(const void *const ctx) { printf("callback\n"); }

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

void print_positional() {
  printf("positional:\n\tlong: %lu\n\tdouble: %f\n\tconst char *: %s\n", u, d,
         l);
}

bool q, w, e, r, t, y;
long u, i, o, p, a, s;
double d, f, g, h, j, k;
const char *l, *z, *x, *c, *v, *b;

struct mbx_opt optv[] = {
    {.type = MBX_OPT_TYPE_BOOL, .shorthand = 'q', .dest = &q},
    {.type = MBX_OPT_TYPE_BOOL, .shorthand = 'w', .dest = &w},
    {.type = MBX_OPT_TYPE_BOOL, .shorthand = 'e', .dest = &e},
    {.type = MBX_OPT_TYPE_BOOL, .longhand = "r", .dest = &r},
    {.type = MBX_OPT_TYPE_BOOL, .longhand = "t", .dest = &t},
    {.type = MBX_OPT_TYPE_BOOL, .longhand = "y", .dest = &y},

    {.type = MBX_OPT_TYPE_LONG, .shorthand = 'u', .dest = &u},
    {.type = MBX_OPT_TYPE_LONG, .shorthand = 'i', .dest = &i},
    {.type = MBX_OPT_TYPE_LONG, .shorthand = 'o', .dest = &o},
    {.type = MBX_OPT_TYPE_LONG, .longhand = "p", .dest = &p},
    {.type = MBX_OPT_TYPE_LONG, .longhand = "a", .dest = &a},
    {.type = MBX_OPT_TYPE_LONG, .longhand = "s", .dest = &s},

    {.type = MBX_OPT_TYPE_DBL, .shorthand = 'd', .dest = &d},
    {.type = MBX_OPT_TYPE_DBL, .shorthand = 'f', .dest = &f},
    {.type = MBX_OPT_TYPE_DBL, .shorthand = 'g', .dest = &g},
    {.type = MBX_OPT_TYPE_DBL, .longhand = "h", .dest = &h},
    {.type = MBX_OPT_TYPE_DBL, .longhand = "j", .dest = &j},
    {.type = MBX_OPT_TYPE_DBL, .longhand = "k", .dest = &k},

    {.type = MBX_OPT_TYPE_STR, .shorthand = 'l', .dest = &l},
    {.type = MBX_OPT_TYPE_STR, .shorthand = 'z', .dest = &z},
    {.type = MBX_OPT_TYPE_STR, .shorthand = 'x', .dest = &x},
    {.type = MBX_OPT_TYPE_STR, .longhand = "c", .dest = &c},
    {.type = MBX_OPT_TYPE_STR, .longhand = "v", .dest = &v},
    {.type = MBX_OPT_TYPE_STR, .longhand = "b", .dest = &b},

    {.shorthand = 'n', .longhand = "callback", .handler.callback = callback},

    {.type = MBX_OPT_TYPE_SUBCOMMAND,
     .longhand = "subcommand",
     .ctx = &subcommand},
    {.type = MBX_OPT_TYPE_LONG, .mods = MBX_OPT_MOD_POSITIONAL, .dest = &u},
    {.type = MBX_OPT_TYPE_DBL, .mods = MBX_OPT_MOD_POSITIONAL, .dest = &d},
    {.type = MBX_OPT_TYPE_STR, .mods = MBX_OPT_MOD_POSITIONAL, .dest = &l},
};

int const optc = ARRAY_LENGTH(optv);

struct mbx_opts subcommand = {.desc = "subcommand", .optc = optc, .optv = optv};
