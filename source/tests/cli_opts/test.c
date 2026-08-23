#define STARK_CLI_OPTS_IMPL
#include "test.h"

#include <stdio.h>
#include <stdlib.h>

void init_env(bool lh) {
  if (lh) {
    putenv("Y=");
    putenv("S=3");
    putenv("K=3.0");
    putenv("B=String3");
  } else {
    putenv("E=");
    putenv("O=3");
    putenv("G=3.0");
    putenv("X=String3");
  }
}

void print_shorthand(void) {
  printf(
      "shorthand:\n\tboolean1: %d\n\tboolean2: %d\n\tboolean3: %d\n\tint64_t1: "
      "%lu\n\tint64_t2: %lu\n\tint64_t3: %lu\n\tfloat641: "
      "%f\n\tfloat642: %f\n\tfloat643: %f\n\tstring1: %s\n\tstring2: "
      "%s\n\tstring3: %s\n",
      q, w, e, u, i, o, d, f, g, l, z, x);
}

void print_longhand(void) {
  printf(
      "longhand:\n\tboolean1: %d\n\tboolean2: %d\n\tboolean3: %d\n\tint64_t1: "
      "%lu\n\tint64_t2: %lu\n\tint64_t3: %lu\n\tfloat641: "
      "%f\n\tfloat642: %f\n\tfloat643: %f\n\tstring1: %s\n\tstring2: "
      "%s\n\tstring3: %s\n",
      r, t, y, p, a, s, h, j, k, c, v, b);
}

void print_positional(void) {
  printf("positional:\n\tint64_t: %lu\n\tfloat64: %f\nstring: %s\n", u, d, l);
}

void print_array(void) {
  printf("array:\n");

  for (int i = 0; i < 6; i++) {
    printf("\tba[%d]: %d\n", i, ba[i]);
    printf("\ti64a[%d]: %ld\n", i, i64a[i]);
    printf("\tf64a[%d]: %g\n", i, f64a[i]);
    printf("\tstra[%d]: %s\n", i, stra[i]);
  }
}

bool q, w, e, r, t, y, ba[6];
int64_t u, i, o, p, a, s, i64a[6];
double d, f, g, h, j, k, f64a[6];
const char *l, *z, *x, *c, *v, *b, *stra[6];

struct stark_cli_opt optv[] = {
    {.type = STARK_CLI_OPT_TYPE_INT64,
     .mods = STARK_CLI_OPT_MOD_POSITIONAL,
     .dest = &u},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64,
     .mods = STARK_CLI_OPT_MOD_POSITIONAL,
     .dest = &d},
    {.type = STARK_CLI_OPT_TYPE_STR,
     .mods = STARK_CLI_OPT_MOD_POSITIONAL,
     .dest = &l},

    {.type = STARK_CLI_OPT_TYPE_SUBCOMMAND,
     .mods = STARK_CLI_OPT_MOD_POSITIONAL,
     .longhand = "subcommand",
     .ctx = &subcommand},

    {.type = STARK_CLI_OPT_TYPE_HELP, .shorthand = 'h', .longhand = "help"},

    {.type = STARK_CLI_OPT_TYPE_BOOL, .shorthand = 'q', .dest = &q},
    {.type = STARK_CLI_OPT_TYPE_BOOL, .shorthand = 'w', .dest = &w},
    {.type = STARK_CLI_OPT_TYPE_BOOL, .shorthand = 'e',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "E",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &e},
    {.type = STARK_CLI_OPT_TYPE_BOOL, .longhand = "r", .dest = &r},
    {.type = STARK_CLI_OPT_TYPE_BOOL, .longhand = "t", .dest = &t},
    {.type = STARK_CLI_OPT_TYPE_BOOL, .longhand = "y",
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "Y",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &y},
    {.type = STARK_CLI_OPT_TYPE_BOOL,
     .mods = STARK_CLI_OPT_MOD_ARRAY,
     .longhand = "ba",
     .dest = ba,
     .arr_len = sizeof(ba) / sizeof(ba[0])},

    {.type = STARK_CLI_OPT_TYPE_INT64,
     .shorthand = 'u',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "U",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &u},
    {.type = STARK_CLI_OPT_TYPE_INT64,
     .shorthand = 'i',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "I",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &i},
    {.type = STARK_CLI_OPT_TYPE_INT64,
     .shorthand = 'o',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "O",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &o},
    {.type = STARK_CLI_OPT_TYPE_INT64, .longhand = "p",
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "P",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &p},
    {.type = STARK_CLI_OPT_TYPE_INT64, .longhand = "a",
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "A",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &a},
    {.type = STARK_CLI_OPT_TYPE_INT64, .longhand = "s",
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "S",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &s},
    {
        .type = STARK_CLI_OPT_TYPE_INT64,
        .mods = STARK_CLI_OPT_MOD_ARRAY,
        .longhand = "i64a",
        .dest = i64a,
        .arr_len = sizeof(i64a) / sizeof(i64a[0]),
    },

    {.type = STARK_CLI_OPT_TYPE_FLOAT64,
     .shorthand = 'd',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "D",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &d},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64,
     .shorthand = 'f',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "F",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &f},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64,
     .shorthand = 'g',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "G",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &g},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64, .longhand = "h",
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "H",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &h},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64, .longhand = "j",
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "J",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &j},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64, .longhand = "k",
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "K",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &k},
    {
        .type = STARK_CLI_OPT_TYPE_FLOAT64,
        .mods = STARK_CLI_OPT_MOD_ARRAY,
        .longhand = "f64a",
        .dest = f64a,
        .arr_len = sizeof(f64a) / sizeof(f64a[0]),
    },

    {.type = STARK_CLI_OPT_TYPE_STR, .shorthand = 'l',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "L",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &l},
    {.type = STARK_CLI_OPT_TYPE_STR, .shorthand = 'z',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "Z",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &z},
    {.type = STARK_CLI_OPT_TYPE_STR, .shorthand = 'x',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "X",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &x},
    {.type = STARK_CLI_OPT_TYPE_STR, .longhand = "c",
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "C",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &c},
    {.type = STARK_CLI_OPT_TYPE_STR, .longhand = "v",
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "V",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &v},
    {.type = STARK_CLI_OPT_TYPE_STR, .longhand = "b",
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = "B",
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &b},
    {
        .type = STARK_CLI_OPT_TYPE_STR,
        .mods = STARK_CLI_OPT_MOD_ARRAY,
        .longhand = "stra",
        .dest = stra,
        .arr_len = sizeof(stra) / sizeof(stra[0]),
    },

    {.type = STARK_CLI_OPT_TYPE_SUBCOMMAND,
     .longhand = "subcommand",
     .ctx = &subcommand},
};

int const optc = sizeof(optv) / sizeof(optv[0]);

struct stark_cli_opts subcommand = {
    .desc = "subcommand", .optc = optc, .optv = optv};
