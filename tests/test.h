#ifndef TEST_H
#define TEST_H

#include "cli_opts.h"

extern bool b;
extern const char *str;
extern int i;
extern long l;
extern float f;
extern double d;
extern struct cli_opt opts[];

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(*x))

void callback(void *ctx);

void test_print();

#endif // TEST_H
