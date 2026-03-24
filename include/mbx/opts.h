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
#ifndef MBX_OPTS_LH_LUT_SIZE
#define MBX_OPTS_LH_LUT_SIZE 128
#endif
#ifndef MBX_OPTS_POS_LUT_SIZE
#define MBX_OPTS_POS_LUT_SIZE 8
#endif

typedef void (*mbx_opt_callback)(const void *const restrict ctx);
typedef bool (*mbx_opt_validator)(const void *const restrict val,
                                  const void *const restrict ctx);
typedef bool (*mbx_opt_assigner)(const char *const restrict str,
                                 void *const restrict dest);

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
  MBX_OPT_TYPE_SUBCOMMAND = 10,

  /* modifiers */
  MBX_OPT_MOD_REQUIRED = 0x0100,
  MBX_OPT_MOD_ARRAY = 0x0200,
  MBX_OPT_MOD_POSITIONAL = 0x0400,

  MBX_OPT_TYPE_MASK = 0xFF,
};

typedef struct mbx_opt {
  uint32_t const type;                 // 4 bytes
  uint8_t arrc;                        // 1 byte
  uint8_t const arrl;                  // 1 byte
  unsigned char const shorthand;       // 1 byte
  uint8_t lens;                        // 1 byte
  char const *const restrict longhand; // 8 bytes
  char const *const restrict alias;    // 8 bytes
  void *dest;                          // 8 bytes
  mbx_opt_assigner const assign;       // 8 bytes
  void *const restrict ctx;            // 8 bytes
  const union {
    mbx_opt_callback const callback;
    mbx_opt_validator const validate;
  } handler;                        // 8 bytes
  char const *const restrict usage; // 8 bytes
} mbx_opt; // fits into one CPU L1 cache line or 64 bytes of memory

typedef struct mbx_opts {
  const char *_token;
  struct mbx_opt *_sh_lut[256];
  struct mbx_opt *_lh_lut[MBX_OPTS_LH_LUT_SIZE];
  struct mbx_opt *_pos_lut[MBX_OPTS_POS_LUT_SIZE];
  const char **_argv;
  int _argc;
  uint8_t _posc;
  const char *const restrict desc;
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
