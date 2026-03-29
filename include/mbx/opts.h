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
#define MBX_COLD __attribute__((cold))
#define MBX_HOT __attribute__((hot, flatten, always_inline))
#else
#define MBX_COLD
#define MBX_HOT
#endif

// To change the size of the corresponding longhand (LH) and positional (POS)
// lookup tables, define these macros before including this header
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
                                 void *const restrict dest, uint8_t const arrc);

enum mbx_opt_type {
  /* regular types */
  MBX_OPT_TYPE_BOOL = 1,
  MBX_OPT_TYPE_INT,
  MBX_OPT_TYPE_LONG,
  MBX_OPT_TYPE_FLOAT,
  MBX_OPT_TYPE_DBL,
  MBX_OPT_TYPE_STR,

  /* builtin types */
  MBX_OPT_TYPE_HELP,

  /* special types */
  MBX_OPT_TYPE_SUBCOMMAND,
};

enum mbx_opt_mod {
  MBX_OPT_MOD_ARRAY = 0x01,
  MBX_OPT_MOD_POSITIONAL = 0x02,
  MBX_OPT_MOD_REQUIRED = 0x04,
  MBX_OPT_MOD_HIDDEN = 0x08,
  MBX_OPT_FOUND = 0x10,
};

typedef struct mbx_opt {
  uint8_t type;                        // 1 byte (4 bits free)
  uint8_t mods;                        // 1 byte (3 bits free)
  uint8_t arrc;                        // 1 byte
  uint8_t const arrl;                  // 1 byte
  char const delim;                    // 1 byte
  unsigned char const shorthand;       // 1 byte
  uint8_t long_len;                    // 1 byte (2 bits free)
  uint8_t alias_len;                   // 1 byte (2 bits free)
  char const *const restrict longhand; // 8 bytes
  char const *const restrict alias;    // 8 bytes
  void *const restrict dest;           // 8 bytes
  mbx_opt_assigner const assign;       // 8 bytes
  const union {
    mbx_opt_callback const callback;
    mbx_opt_validator const validate;
  } handler;                        // 8 bytes
  void *const restrict ctx;         // 8 bytes
  char const *const restrict usage; // 8 bytes
} mbx_opt; // fits into one CPU L1 cache line or 64 bytes of memory

// If you don't define a description (desc) then make sure to zero the struct.
// using `= {0}`
typedef struct mbx_opts {
  struct mbx_opt *_sh_lut[256];
  struct mbx_opt *_lh_lut[MBX_OPTS_LH_LUT_SIZE];
  struct mbx_opt *_pos_lut[MBX_OPTS_POS_LUT_SIZE];
  char const *restrict _token;
  char const **_argv;
  struct mbx_opt *const optv;
  char const *const restrict desc;
  int _argc;
  int const optc;
  uint8_t _posc;
  bool _verified;
} mbx_opts;

MBX_COLD bool mbx_opts_init(struct mbx_opts *const restrict opts);
MBX_COLD bool mbx_opts_parse(struct mbx_opts *const restrict app,
                             int const argc, char const **const argv);

#ifdef __cplusplus
}
#endif

#endif // MBX_OPTS_H
