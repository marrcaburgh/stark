#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdbool.h>

#include "mbx/opts.h"

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

extern bool q, w, e, r, t, y;
extern long u, i, o, p, a, s;
extern double d, f, g, h, j, k;
extern const char *l, *z, *x, *c, *v, *b;

extern struct mbx_opt opt[];
extern const int optc;

void callback(const void *const ctx);
bool validate(const char *const str, const void *const ctx);

void print_shorthand();
void print_longhand();

#endif // BENCHMARK_H
