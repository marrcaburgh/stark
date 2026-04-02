#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdbool.h>

#include "mbx/opts.h"

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

extern bool q, w, e, r, t, y;
extern long u, i, o, p, a, s;
extern double d, f, g, h, j, k;
extern char const *l, *z, *x, *c, *v, *b;

extern struct mbx_opts subcommand;
extern struct mbx_opt optv[];
extern int const optc;

void callback(const void *const ctx);
bool validate(const char *const str, const void *const ctx);

void print_shorthand(void);
void print_longhand(void);
void print_positional(void);

#endif // BENCHMARK_H
