#ifndef MB_CLITE_OPTS_H
#define MB_CLITE_OPTS_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mb_opts_callback)(const void *const ctx);
typedef bool (*mb_opts_validator)(const char *const val, const void *const ctx);
typedef bool (*mb_opts_assigner)(const char *const str, void *const dest);

enum mb_opts_type {
  /* regular types */
  MB_OPT_BOOL,
  MB_OPT_INT,
  MB_OPT_LONG,
  MB_OPT_FLOAT,
  MB_OPT_DBL,
  MB_OPT_STR,

  /* special types */
  MB_OPT_CALLBACK,
  MB_OPT_CUSTOM,

  /* modifiers */
  MB_OPT_REQUIRED = 1 << 8,
  MB_OPT_ARRAY = 1 << 9,

  /* builtin types */
  MB_OPT_HELP,
  MB_OPT_END
};

typedef struct mb_opt {
  enum mb_opts_type type;
  const char shorthand;
  const char *const longhand;
  const char *const alias;
  void *const dest;
  const mb_opts_assigner assign;
  const uint16_t elem_size;
  const void *const ctx;
  union {
    const mb_opts_callback cb;
    const mb_opts_validator valid;
  } handler;
  const char *const help;
} mb_opt;

typedef struct mb_opts {
  const char *token;
  const struct mb_opt *opts;
  const char **argv;
  int argc;
  const char *desc;
} mb_opts;

#define MB_OPT(sh, lh, typ, dst, hlp, ...)                                     \
  {.shorthand = sh,                                                            \
   .longhand = lh,                                                             \
   .type = typ,                                                                \
   .help = hlp,                                                                \
   .dest = dst,                                                                \
   ##__VA_ARGS__}

#define MB_OPT_CALLBACK(sh, lh, callback, context, hlp)                        \
  {.shorthand = sh,                                                            \
   .longhand = lh,                                                             \
   .type = MB_OPT_CALLBACK,                                                    \
   .handler.cb = callback,                                                     \
   .ctx = context,                                                             \
   .help = hlp}

#define MB_OPT_LIST(...)                                                       \
  __VA_ARGS__, MB_OPT('h', "help", MB_OPT_HELP, NULL, "prints this help"), {   \
    .type = MB_OPT_END                                                         \
  }

bool mb_opts_init(struct mb_opts *const app, struct mb_opt *const opts,
                  const char *const desc);
bool mb_opts_parse(struct mb_opts *const app, const int argc,
                   const char **const argv);

#ifdef __cplusplus
}
#endif

#endif // MB_CLITE_OPTS_H
