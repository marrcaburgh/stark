#include "test.h"

#include <stdbool.h>
#include <stdio.h>

bool b = false;
const char *str = "";
int i = 0;
long l = 0L;
float f = 0.0f;
double d = 0.0;
FILE *file;

void callback(const void *const ctx) { printf("callback\n"); }
bool validate(const char *const str, const void *const ctx) { return false; }

struct mbx_opt opts[] = {
    // clang-format off
      MBX_OPT('b', "boolean", MBX_OPT_TYPE_BOOL, &b, "a flag/boolean"),
      MBX_OPT('s', "string", MBX_OPT_TYPE_STR, &str, "a string"),
      MBX_OPT('i', "integer", MBX_OPT_TYPE_INT, &i, "an integer"),
      MBX_OPT('l', "long", MBX_OPT_TYPE_LONG, &l, "a long"),
      MBX_OPT('f', "float", MBX_OPT_TYPE_FLOAT, &f, "a float"),
      MBX_OPT('d', "double", MBX_OPT_TYPE_DBL, &d, "a double"),
      MBX_OPT_CALLBACK('c', "callback", callback, NULL, "a callback")
    // clang-format on
};

void test_print() {
  printf("b = %d\n", b);
  printf("str = %s\n", str);
  printf("i = %d\n", i);
  printf("l = %lu\n", l);
  printf("f = %f\n", f);
  printf("d = %f\n", d);
}
