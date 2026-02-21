#ifndef MB_CLITE_OPTS_H
#define MB_CLITE_OPTS_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
#define MB_ALIGN64 __attribute__((aligned(64)))
#define likely(x) __builtin_expect(!!(x), 1)
#define MB_COLD __attribute__((cold))
#elif defined(_MSC_VER)
#define MB_ALIGN64 __declspec(align(64))
#else
#define MB_ALIGN64
#define MB_COLD
#endif

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

#define mb_opts_init(opts_ptr) likely(_mb_opts_init(opts_ptr))

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

MB_ALIGN64 typedef struct mb_opt {
  uint16_t type;            // 2 bytes
  const char shorthand;     // 1 byte
  const uint16_t elem_size; // 2 bytes
  uint8_t lens;
  const char *const longhand;    // 8 bytes
  const char *const alias;       // 8 bytes
  void *const dest;              // 8 bytes
  const mb_opts_assigner assign; // 8 bytes
  const void *const ctx;         // 8 bytes
  union {
    const mb_opts_callback cb;
    const mb_opts_validator valid;
  } handler;              // 8 bytes
  const char *const help; // 8 bytes
} mb_opt;                 // fits into one CPU L1 cache line or 64b

typedef struct mb_opts {
  const char *_token;
  const struct mb_opt *const opts;
  const char **_argv;
  int _argc;
  const char *desc;
  bool verified;
} mb_opts;

bool _mb_opts_init(mb_opts *const app);
bool mb_opts_parse(struct mb_opts *const app, const int argc,
                   const char **const argv);

#ifdef __cplusplus
}
#endif

#endif // MB_CLITE_OPTS_H
