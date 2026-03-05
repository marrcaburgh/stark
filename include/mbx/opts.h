#ifndef MBX_OPTS_H
#define MBX_OPTS_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define MB_COLD __attribute__((cold))
#define MB_HOT __attribute__((hot, flatten))
#else
#define MB_COLD
#define MB_HOT
#endif
#ifndef MB_LH_LUT_SIZE
#define MB_LH_LUT_SIZE 128
#endif

#define MBX_OPT(sh, lh, typ, dst, usg, ...)                                    \
  {.shorthand = (sh),                                                          \
   .longhand = (lh),                                                           \
   .type = (typ),                                                              \
   .dest = (dst),                                                              \
   .usage = (usg),                                                             \
   ##__VA_ARGS__}

#define MBX_OPT_CALLBACK(sh, lh, callback, context, usg)                       \
  {.shorthand = (sh),                                                          \
   .longhand = (lh),                                                           \
   .type = MBX_OPT_TYPE_CALLBACK,                                              \
   .handler.callback = (callback),                                             \
   .ctx = (context),                                                           \
   .usage = (usg)}

#define MBX_OPT_POS(typ, dst, usg, ...)                                        \
  {.type = (typ) | MBX_OPT_POSITIONAL,                                         \
   .dest = (dst),                                                              \
   .usage = (usg),                                                             \
   ##__VA_ARGS__}

#define MBX_OPT_HELP                                                           \
  {.shorthand = 'h', .longhand = "help", .type = MBX_OPT_TYPE_HELP}

typedef void (*mbx_opt_callback)(const void *const ctx);
typedef bool (*mbx_opt_validator)(const char *const val, const void *const ctx);
typedef bool (*mbx_opt_assigner)(const char *const str, void *const dest);

enum mbx_opt_type {
  /* regular types */
  MBX_OPT_TYPE_BOOL = 1,
  MBX_OPT_TYPE_INT = 2,
  MBX_OPT_TYPE_LONG = 3,
  MBX_OPT_TYPE_FLOAT = 4,
  MBX_OPT_TYPE_DBL = 5,
  MBX_OPT_TYPE_STR = 6,

  /* bultin types */
  MBX_OPT_TYPE_HELP = 7,

  /* special types */
  MBX_OPT_TYPE_CALLBACK = 8,
  MBX_OPT_TYPE_CUSTOM = 9,

  /* modifiers */
  MBX_OPT_REQUIRED = 0x0100,
  MBX_OPT_ARRAY = 0x0200,
  MBX_OPT_POSITIONAL = 0x0300,

  MBX_OPT_TYPE_MASK = 0xFF
};

typedef struct mbx_opt {
  const uint32_t type;                 // 4 bytes
  const uint16_t elem_size;            // 2 bytes
  const unsigned char shorthand;       // 1 byte
  uint8_t lens;                        // 1 byte
  const char *const restrict longhand; // 8 bytes
  const char *const restrict alias;    // 8 bytes
  void *const dest;                    // 8 bytes
  const mbx_opt_assigner assign;       // 8 bytes
  const void *const restrict ctx;      // 8 bytes
  const union {
    const mbx_opt_callback callback;
    const mbx_opt_validator validate;
  } handler;                        // 8 bytes
  const char *const restrict usage; // 8 bytes
} mbx_opt; // fits into one CPU L1 cache line or 64 bytes of memory

typedef struct mbx_opts {
  const char *_token;
  const struct mbx_opt *_sh_lut[256];
  // const struct mbx_opt_lh_lut _lh_lut;
  const struct mbx_opt *_lh_lut[MB_LH_LUT_SIZE];
  const char **_argv;
  int _argc;
  const char *const desc;
  bool _verified;
} mbx_opts;

MB_COLD bool mbx_opts_init(struct mbx_opts *const restrict opts, const int optc,
                           struct mbx_opt *const restrict optv);
MB_COLD bool mbx_opts_parse(struct mbx_opts *const restrict app, const int argc,
                            const char **const argv);

#ifdef __cplusplus
}
#endif

#endif // MBX_OPTS_H
