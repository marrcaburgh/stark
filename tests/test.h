#ifndef TEST_H
#define TEST_H

#include "mbx/opts.h"

extern bool b;
extern const char *str;
extern int i;
extern long l;
extern float f;
extern double d;
extern struct mbx_opt opts[7];

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(*x))

void test_print();

#endif // TEST_H
