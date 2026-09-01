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
  printf("positional:\n\tint64_t: %lu\n\tfloat64: %f\n\tstring: %s\n", u, d, l);
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
     .longhand = (char const *const[]){"subcommand", NULL},
     .ctx = &subcommand},

    {.type = STARK_CLI_OPT_TYPE_HELP,
     .shorthand = 'h',
     .longhand = (char const *const[]){"help", NULL}},

    {.type = STARK_CLI_OPT_TYPE_BOOL, .shorthand = 'q', .dest = &q},
    {.type = STARK_CLI_OPT_TYPE_BOOL, .shorthand = 'w', .dest = &w},
    {.type = STARK_CLI_OPT_TYPE_BOOL,
     .shorthand = 'e',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"E", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &e},
    {.type = STARK_CLI_OPT_TYPE_BOOL,
     .longhand = (char const *const[]){"r", NULL},
     .dest = &r},
    {.type = STARK_CLI_OPT_TYPE_BOOL,
     .longhand = (char const *const[]){"t", NULL},
     .dest = &t},
    {.type = STARK_CLI_OPT_TYPE_BOOL,
     .longhand = (char const *const[]){"y", NULL},
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"Y", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &y},
    {.type = STARK_CLI_OPT_TYPE_BOOL,
     .mods = STARK_CLI_OPT_MOD_ARRAY,
     .longhand = (char const *const[]){"ba", NULL},
     .dest = ba,
     .arr_len = sizeof(ba) / sizeof(ba[0])},

    {.type = STARK_CLI_OPT_TYPE_INT64,
     .shorthand = 'u',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"U", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &u},
    {.type = STARK_CLI_OPT_TYPE_INT64,
     .shorthand = 'i',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"I", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &i},
    {.type = STARK_CLI_OPT_TYPE_INT64,
     .shorthand = 'o',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"O", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &o},
    {.type = STARK_CLI_OPT_TYPE_INT64,
     .longhand = (char const *const[]){"p", NULL},
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"P", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &p},
    {.type = STARK_CLI_OPT_TYPE_INT64,
     .longhand = (char const *const[]){"a", NULL},
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"A", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &a},
    {.type = STARK_CLI_OPT_TYPE_INT64,
     .longhand = (char const *const[]){"s", NULL},
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"S", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &s},
    {
        .type = STARK_CLI_OPT_TYPE_INT64,
        .mods = STARK_CLI_OPT_MOD_ARRAY,
        .longhand = (char const *const[]){"i64a", NULL},
        .dest = i64a,
        .arr_len = sizeof(i64a) / sizeof(i64a[0]),
    },

    {.type = STARK_CLI_OPT_TYPE_FLOAT64,
     .shorthand = 'd',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"D", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &d},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64,
     .shorthand = 'f',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"F", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &f},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64,
     .shorthand = 'g',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"G", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &g},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64,
     .longhand = (char const *const[]){"h", NULL},
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"H", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &h},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64,
     .longhand = (char const *const[]){"j", NULL},
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"J", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &j},
    {.type = STARK_CLI_OPT_TYPE_FLOAT64,
     .longhand = (char const *const[]){"k", NULL},
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"K", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &k},
    {
        .type = STARK_CLI_OPT_TYPE_FLOAT64,
        .mods = STARK_CLI_OPT_MOD_ARRAY,
        .longhand = (char const *const[]){"f64a", NULL},
        .dest = f64a,
        .arr_len = sizeof(f64a) / sizeof(f64a[0]),
    },

    {.type = STARK_CLI_OPT_TYPE_STR,
     .shorthand = 'l',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"L", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &l},
    {.type = STARK_CLI_OPT_TYPE_STR,
     .shorthand = 'z',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"Z", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &z},
    {.type = STARK_CLI_OPT_TYPE_STR,
     .shorthand = 'x',
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"X", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &x},
    {.type = STARK_CLI_OPT_TYPE_STR,
     .longhand = (char const *const[]){"c", NULL},
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"C", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &c},
    {.type = STARK_CLI_OPT_TYPE_STR,
     .longhand = (char const *const[]){"v", NULL},
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"V", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &v},
    {.type = STARK_CLI_OPT_TYPE_STR,
     .longhand = (char const *const[]){"b", NULL},
#ifdef STARK_CLI_OPTS_ENABLE_ENV
     .env = (char const *const[]){"B", NULL},
#endif // STARK_CLI_OPTS_ENABLE_ENV
     .dest = &b},
    {
        .type = STARK_CLI_OPT_TYPE_STR,
        .mods = STARK_CLI_OPT_MOD_ARRAY,
        .longhand = (char const *const[]){"stra", NULL},
        .dest = stra,
        .arr_len = sizeof(stra) / sizeof(stra[0]),
    },

    {.type = STARK_CLI_OPT_TYPE_SUBCOMMAND,
     .longhand = (char const *const[]){"subcommand", NULL},
     .ctx = &subcommand},
};

int const optc = sizeof(optv) / sizeof(optv[0]);

struct stark_cli_opts subcommand = {
    .desc = "subcommand", .optc = optc, .optv = optv};
