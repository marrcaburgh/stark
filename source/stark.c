#include "stark/cli_opts.hi"

#include <stdio.h>

char const *test_infile;

struct stark_cli_opt global_opts = {
	{ .type = STARK_CLI_OPT_TYPE_HELP, shorthand = 'h', .longhand = "help", .usage = "Prints this help page" },
}

struct stark_cli_opts test = {
  {.type = STARK_CLI_OPT_TYPE_STR, .shorthand = 'f', .longhand = "file", .dest = &test_infile, .usage = "The test file to run"}
};

struct stark_cli_opt opts[] = {
  {.type = STARK_CLI_OPT_TYPE_SUBCOMMAND, .mods = STARK_CLI_OPT_MOD_POSITIONAL, .longhand = "test", .usage = "Run a test"}
};

int main(int argc, char **argv) {
	return 0;
}
