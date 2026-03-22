#include "test.h"

int main() {
  // clang-format off
  const char *argv[] = {
    "longhand",
    "--r",
    "--p=1",
    "--a", "2",
    "--h=1.0",
    "--j", "2.0",
    "--c=String1",
    "--v", "String2"
  };
  // clang-format on

  static struct mbx_opts opts = {.desc = "longhand test"};

  if (!mbx_opts_init(&opts, optc, optv)) {
    return 1;
  }

  if (!mbx_opts_parse(&opts, ARRAY_LENGTH(argv), argv)) {
    return 2;
  }

  print_longhand();

  return 0;
}
