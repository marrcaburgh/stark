#ifndef CLI_OPTS_H
#define CLI_OPTS_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*cli_opt_callback)(void *ctx);
typedef bool (*cli_opt_validator)(const char *str, void *ctx);
typedef bool (*cli_opt_assigner)(const char *str, void *dest);

enum cli_opt_type {
  /* regular types */
  CLI_OPT_TYPE_BOOL,
  CLI_OPT_TYPE_INT,
  CLI_OPT_TYPE_LONG,
  CLI_OPT_TYPE_FLOAT,
  CLI_OPT_TYPE_DBL,
  CLI_OPT_TYPE_STR,

  /* special types */
  CLI_OPT_TYPE_ACTION,
  CLI_OPT_TYPE_CUSTOM,

  /* modifiers */
  CLI_OPT_REQ,
  CLI_OPT_ARRAY,
  CLI_OPT_NON_NEG,
  CLI_OPT_NOT_EMPT,

  /* builtin types */
  CLI_OPT_HELP,
  CLI_OPT_END
};

typedef struct cli_opt {
  void *const dest;
  const char *longhand;
  const char *alias;
  enum cli_opt_type type;
  const char shorthand;
  const char *help;
  void *const ctx;
  const cli_opt_callback callback;
  const cli_opt_validator validator;
  const cli_opt_assigner assign;
} cli_opt;

typedef struct cli_opts {
  const struct cli_opt *opts;
  const char **argv;
  const char *token;
  int argc;
  const char *desc;
} cli_opts;

#define CLI_OPT(sh, lh, typ, dst, hlp, ...)                                    \
  {.shorthand = sh,                                                            \
   .longhand = lh,                                                             \
   .type = typ,                                                                \
   .dest = dst,                                                                \
   .help = hlp,                                                                \
   ##__VA_ARGS__}

#define CLI_OPT_CALLBACK(sh, lh, act, context, hlp)                            \
  {.shorthand = sh,                                                            \
   .longhand = lh,                                                             \
   .type = CLI_OPT_TYPE_ACTION,                                                \
   .action = act,                                                              \
   .ctx = context,                                                             \
   .help = hlp}

#define CLI_OPT_LIST(...)                                                      \
  __VA_ARGS__, CLI_OPT('h', "help", CLI_OPT_HELP, NULL, "prints this help"), { \
    .type = CLI_OPT_END                                                        \
  }

bool cli_opts_init(struct cli_opts *const app, struct cli_opt *const opts,
                   const char *const desc);
bool cli_opts_parse(struct cli_opts *const app, const int argc,
                    const char **const argv);

#ifdef __cplusplus
}
#endif

#endif // CLI_OPTS_H
