#include "test.h"
#include <stdio.h>

bool b = false;
const char *str = "";
int i = 0;
long l = 0L;
float f = 0.0f;
double d = 0.0;
FILE *file;

void action(void *ctx) { printf("action\n"); }
bool validate(const char *str, void *ctx) {}

CLI_OPTS(opts,
         // clang-format off
      CLI_OPT('b', "boolean", CLI_OPT_TYPE_BOOL, &b, "a flag/boolean"),
      CLI_OPT('s', "string", CLI_OPT_TYPE_STR, &str, "a string"),
      CLI_OPT('i', "integer", CLI_OPT_TYPE_INT, &i, "an integer", .validate = validate),
      CLI_OPT('l', "long", CLI_OPT_TYPE_LONG, &l, "a long"),
      CLI_OPT('f', "float", CLI_OPT_TYPE_FLOAT, &f, "a float"),
      CLI_OPT('d', "double", CLI_OPT_TYPE_DBL, &d, "a double"),
      CLI_OPT_ACTION('a', "action", action, NULL, "an action flag")
         // clang-format on
);

void test_print() {
  printf("b = %d\n", b);
  printf("str = %s\n", str);
  printf("i = %d\n", i);
  printf("l = %lu\n", l);
  printf("f = %f\n", f);
  printf("d = %f\n", d);
}
