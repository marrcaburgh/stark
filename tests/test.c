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

void callback(const void *const ctx) { printf("action\n"); }
bool validate(const char *const str, const void *const ctx) { return false; }

struct mb_opt opts[] = {
    // clang-format off
    MB_OPT_LIST(
      MB_OPT('b', "boolean", MB_OPT_BOOL, &b, "a flag/boolean"),
      MB_OPT('s', "string", MB_OPT_STR, &str, "a string"),
      MB_OPT('i', "integer", MB_OPT_INT, &i, "an integer"),
      MB_OPT('l', "long", MB_OPT_LONG, &l, "a long"),
      MB_OPT('f', "float", MB_OPT_FLOAT, &f, "a float"),
      MB_OPT('d', "double", MB_OPT_DBL, &d, "a double"),
      MB_OPT_CALLBACK('c', "callback", callback, NULL, "a callback")
      )
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
