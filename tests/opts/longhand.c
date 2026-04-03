#include "test.h"

int main(void) {
  // clang-format off
  char const *argv[] = {
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

  struct stark_opts opts = {
      .desc = "longhand test", .optc = optc, .optv = optv};

  if (!stark_opts_init(&opts)) {
    return 1;
  }

  if (!stark_opts_parse(&opts, ARRAY_LENGTH(argv), argv)) {
    return 2;
  }

  print_longhand();

  return 0;
}
