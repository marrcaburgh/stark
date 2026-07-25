#ifndef TEST_H
#define TEST_H

#include <stdbool.h>

#include "stark/cli_opts.h"

extern bool q, w, e, r, t, y, ba[];
extern long u, i, o, p, a, s, i64a[];
extern double d, f, g, h, j, k, f64a[];
extern char const *l, *z, *x, *c, *v, *b, *stra[];

extern struct stark_cli_opts subcommand;
extern struct stark_cli_opt optv[];
extern int const optc;

void init_env(bool lh);
void print_shorthand(void);
void print_longhand(void);
void print_positional(void);
void print_array(void);

#endif // TEST_H
