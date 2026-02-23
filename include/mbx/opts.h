#ifndef MBX_OPTS_H
#define MBX_OPTS_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MB_LH_LUT_SIZE
#define MB_LH_LUT_SIZE 128
#endif

#if defined(__GNUC__) || defined(__clang__)
#define MB_ALIGN64 __attribute__((aligned(64)))
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define MB_COLD __attribute__((cold))
#define MB_HOT __attribute__((hot, flatten, always_inline))
#elif defined(_MSC_VER)
#define MB_ALIGN64 __declspec(align(64))
#else
#define MB_ALIGN64
#define MB_COLD
#endif

#define MBX_OPT_UNKNOWN 2
#define MBX_OPT_ASSIGN_FAILED 1

#define MBX_OPT(sh, lh, typ, dst, usg, ...)                                    \
  {.shorthand = sh,                                                            \
   .longhand = lh,                                                             \
   .type = typ,                                                                \
   .dest = dst,                                                                \
   .usage = usg,                                                               \
   ##__VA_ARGS__}

#define MBX_OPT_CALLBACK(sh, lh, callback, context, usg)                       \
  {.shorthand = sh,                                                            \
   .longhand = lh,                                                             \
   .type = MBX_OPT_TYPE_CALLBACK,                                              \
   .handler.callback = callback,                                               \
   .ctx = context,                                                             \
   .usage = usg}

#define MBX_OPT_HELP                                                           \
  {.shorthand = 'h', .longhand = "help", .type = MBX_OPT_HELP}

#define mbx_opts_init(opts_ptr, opt, optc)                                     \
  likely(_mbx_opts_init(opts_ptr, opt, optc))

typedef void (*mbx_opt_callback)(const void *const ctx);
typedef bool (*mbx_opt_validator)(const char *const val, const void *const ctx);
typedef bool (*mbx_opt_assigner)(const char *const str, void *const dest);

enum mbx_opt_type {
  /* regular types */
  MBX_OPT_TYPE_BOOL,
  MBX_OPT_TYPE_INT,
  MBX_OPT_TYPE_LONG,
  MBX_OPT_TYPE_FLOAT,
  MBX_OPT_TYPE_DBL,
  MBX_OPT_TYPE_STR,

  /* special types */
  MBX_OPT_TYPE_CALLBACK,
  MBX_OPT_TYPE_CUSTOM,

  /* modifiers */
  MBX_OPT_REQUIRED = 1 << 8,
  MBX_OPT_ARRAY = 1 << 9,

  /* builtin types */
  MBX_OPT_TYPE_HELP,
};

MB_ALIGN64 typedef struct mbx_opt {
  uint16_t type;                 // 2 bytes
  const unsigned char shorthand; // 1 byte
  const uint16_t elem_size;      // 2 bytes
  uint8_t lens;                  // 1 byte
  const char *const longhand;    // 8 bytes
  const char *const alias;       // 8 bytes
  void *const dest;              // 8 bytes
  const mbx_opt_assigner assign; // 8 bytes
  const void *const ctx;         // 8 bytes
  const union {
    const mbx_opt_callback callback;
    const mbx_opt_validator validate;
  } handler;               // 8 bytes
  const char *const usage; // 8 bytes
} mb_opt;                  // fits into one CPU L1 cache line or 64b

typedef struct mbx_opts {
  const char *_token;
  const struct mbx_opt *sh_lut[256];
  const struct mbx_opt *lh_lut[MB_LH_LUT_SIZE];
  const char **_argv;
  int _argc;
  const char *desc;
  bool verified;
} mb_opts;

bool _mbx_opts_init(struct mbx_opts *const restrict app,
                    struct mbx_opt *const opts, const size_t optsc);
bool mbx_opts_parse(struct mbx_opts *const restrict app, const int argc,
                    const char **const argv);

#ifdef __cplusplus
}
#endif

#endif // MBX_OPTS_H
