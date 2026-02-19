#ifndef CLI_OPTS_H
#define CLI_OPTS_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*cli_opt_action)(void *ctx);
typedef bool (*cli_opt_validate)(const char *str, void *ctx);
typedef bool (*cli_opt_parser)(const char *str, void *dest);

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
  const cli_opt_action action;
  const cli_opt_validate validate;
  const cli_opt_parser parser;
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

#define CLI_OPT_ACTION(sh, lh, act, context, hlp)                              \
  {.shorthand = sh,                                                            \
   .longhand = lh,                                                             \
   .type = CLI_OPT_TYPE_ACTION,                                                \
   .action = act,                                                              \
   .ctx = context,                                                             \
   .help = hlp}

#if __STDC_VERSION__ >= 201112L
#define CLI_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#else
#define CLI_STATIC_ASSERT(cond, msg)                                           \
  typedef char static_assertion_##__LINE__[(cond) ? 1 : -1]
#endif

#define CLI_OPTS(name, ...)                                                    \
  struct cli_opt name[] = {                                                    \
      __VA_ARGS__,                                                             \
      CLI_OPT('h', "help", CLI_OPT_HELP, NULL, "prints this help"),            \
      {.type = CLI_OPT_END}};                                                  \
  CLI_STATIC_ASSERT((sizeof((struct cli_opt[]){__VA_ARGS__}) > 0),             \
                    "cli_opts error: options list cannot be empty")

void cli_opts_init(struct cli_opts *const app, struct cli_opt *const opts,
                   const char *const desc);
bool cli_opts_parse(struct cli_opts *const app, const int argc,
                    const char **const argv);

#ifdef __cplusplus
}
#endif

#endif // CLI_OPTS_H
