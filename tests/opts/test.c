#include "test.h"

#include <stdio.h>

void callback(STARK_UNUSED const void *const ctx) { printf("callback\n"); }

void print_shorthand(void) {
  printf("shorthand:\n\tbool: %d\n\tlong1: %lu\n\tlong2: %lu\n\tdouble1: "
         "%f\n\tdouble2: %f\n\tconst char *1: %s\n\tconst char *2: %s\n",
         q, u, i, d, f, l, z);
}

void print_longhand(void) {
  printf("longhand:\n\tbool: %d\n\tlong1: %lu\n\tlong2: %lu\n\tdouble1: "
         "%f\n\tdouble2: %f\n\tconst char *1: %s\n\tconst char *2: %s\n",
         r, p, a, h, j, c, v);
}

void print_positional(void) {
  printf("positional:\n\tlong: %lu\n\tdouble: %f\n\tconst char *: %s\n", u, d,
         l);
}

bool q, w, e, r, t, y;
long u, i, o, p, a, s;
double d, f, g, h, j, k;
const char *l, *z, *x, *c, *v, *b;

struct stark_opt optv[] = {
    {.type = STARK_OPT_TYPE_BOOLEAN, .shorthand = 'q', .dest = &q},
    {.type = STARK_OPT_TYPE_BOOLEAN, .shorthand = 'w', .dest = &w},
    {.type = STARK_OPT_TYPE_BOOLEAN, .shorthand = 'e', .dest = &e},
    {.type = STARK_OPT_TYPE_BOOLEAN, .longhand = "r", .dest = &r},
    {.type = STARK_OPT_TYPE_BOOLEAN, .longhand = "t", .dest = &t},
    {.type = STARK_OPT_TYPE_BOOLEAN, .longhand = "y", .dest = &y},

    {.type = STARK_OPT_TYPE_LONG, .shorthand = 'u', .dest = &u},
    {.type = STARK_OPT_TYPE_LONG, .shorthand = 'i', .dest = &i},
    {.type = STARK_OPT_TYPE_LONG, .shorthand = 'o', .dest = &o},
    {.type = STARK_OPT_TYPE_LONG, .longhand = "p", .dest = &p},
    {.type = STARK_OPT_TYPE_LONG, .longhand = "a", .dest = &a},
    {.type = STARK_OPT_TYPE_LONG, .longhand = "s", .dest = &s},

    {.type = STARK_OPT_TYPE_DOUBLE, .shorthand = 'd', .dest = &d},
    {.type = STARK_OPT_TYPE_DOUBLE, .shorthand = 'f', .dest = &f},
    {.type = STARK_OPT_TYPE_DOUBLE, .shorthand = 'g', .dest = &g},
    {.type = STARK_OPT_TYPE_DOUBLE, .longhand = "h", .dest = &h},
    {.type = STARK_OPT_TYPE_DOUBLE, .longhand = "j", .dest = &j},
    {.type = STARK_OPT_TYPE_DOUBLE, .longhand = "k", .dest = &k},

    {.type = STARK_OPT_TYPE_STRING, .shorthand = 'l', .dest = &l},
    {.type = STARK_OPT_TYPE_STRING, .shorthand = 'z', .dest = &z},
    {.type = STARK_OPT_TYPE_STRING, .shorthand = 'x', .dest = &x},
    {.type = STARK_OPT_TYPE_STRING, .longhand = "c", .dest = &c},
    {.type = STARK_OPT_TYPE_STRING, .longhand = "v", .dest = &v},
    {.type = STARK_OPT_TYPE_STRING, .longhand = "b", .dest = &b},

    {.shorthand = 'n', .longhand = "callback", .handler.callback = callback},

    {.type = STARK_OPT_TYPE_SUBCOMMAND,
     .longhand = "subcommand",
     .ctx = &subcommand},
    {.type = STARK_OPT_TYPE_LONG, .mods = STARK_OPT_MOD_POSITIONAL, .dest = &u},
    {.type = STARK_OPT_TYPE_DOUBLE,
     .mods = STARK_OPT_MOD_POSITIONAL,
     .dest = &d},
    {.type = STARK_OPT_TYPE_STRING,
     .mods = STARK_OPT_MOD_POSITIONAL,
     .dest = &l},
};

int const optc = ARRAY_LENGTH(optv);

struct stark_opts subcommand = {
    .desc = "subcommand", .optc = optc, .optv = optv};
