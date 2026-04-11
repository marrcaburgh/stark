//
// stark - a C99+ utility library - stark_opts - a blazing-fast feature-full
// command-line parser
// Copyright (C) 2026  marrcaburgh
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
//

#ifndef STARK_OPTS_H
#define STARK_OPTS_H

#include "stark/core.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// To change the size of the corresponding longhand (LH) and positional (POS)
// lookup tables, define these macros before including this header
#ifndef STARK_OPTS_LH_LUT_SIZE
#define STARK_OPTS_LH_LUT_SIZE 128
#endif
#ifndef STARK_OPTS_POS_LUT_SIZE
#define STARK_OPTS_POS_LUT_SIZE 8
#endif

typedef enum stark_opts_err_type {
  STARK_OPTS_ERR_SUCCESS,
  STARK_OPTS_ERR_UNKNOWN_OPTION,
  STARK_OPTS_ERR_ASSIGN_FAILED,
  STARK_OPTS_ERR_ARRAY_OUT_OF_BOUNDS,
  STARK_OPTS_ERR_REQUIRED_NOT_FOUND,
  STARK_OPTS_ERR_NULL_OR_INVALID,
  STARK_OPTS_ERR_NOT_VERIFIED
} stark_opts_err_type;

typedef enum stark_opt_type {
  /* regular types */
  STARK_OPT_TYPE_BOOLEAN = 1,
  STARK_OPT_TYPE_INTEGER,
  STARK_OPT_TYPE_LONG,
  STARK_OPT_TYPE_FLOAT,
  STARK_OPT_TYPE_DOUBLE,
  STARK_OPT_TYPE_STRING,

  /* builtin types */
  STARK_OPT_TYPE_HELP,

  /* special types */
  STARK_OPT_TYPE_SUBCOMMAND,
} stark_opt_type;

typedef enum stark_opt_mod_type {
  STARK_OPT_MOD_ARRAY = 0x01,
  STARK_OPT_MOD_POSITIONAL = 0x02,
  STARK_OPT_MOD_REQUIRED = 0x04,
  STARK_OPT_MOD_HIDDEN = 0x08,
  STARK_OPT_MOD_SET_FALSE = 0x10,
  STARK_OPT_FOUND = 0x80,
} stark_opt_mod_type;

typedef struct stark_opt {
  uint8_t type;                  // 1 byte (4 bits free: 0x10, 0x20, 0x40, 0x80)
  uint8_t mods;                  // 1 byte (2 bits free: 0x20, 0x40)
  uint8_t arrc;                  // 1 byte
  uint8_t const arrl;            // 1 byte
  char const delim;              // 1 byte
  unsigned char const shorthand; // 1 byte
  uint8_t _long_len;             // 1 byte (2 bits free: 0x40, 0x80)
  uint8_t _alias_len;            // 1 byte (2 bits free: 0x40, 0x80)
  char const *const restrict longhand; // 8 bytes
  char const *const restrict alias;    // 8 bytes
  void *const restrict dest;           // 8 bytes
  bool (*const assign)(char const *const restrict str,
                       void *const restrict dest, uint8_t const arrc);
  const union {
    void (*const callback)(const void *const restrict ctx);
    bool (*const validate)(const void *const restrict val,
                           const void *const restrict ctx);
  } handler;                        // 8 bytes
  void *const restrict ctx;         // 8 bytes
  char const *const restrict usage; // 8 bytes
} stark_opt_t; // fits into one CPU L1 cache line or 64 bytes of memory

typedef struct stark_opts {
  struct stark_opt *_sh_lut[256];
  struct stark_opt *_lh_lut[STARK_OPTS_LH_LUT_SIZE];
  struct stark_opt *_pos_lut[STARK_OPTS_POS_LUT_SIZE];
  struct stark_opt *_psc_lut[1];
  char const *restrict _token;
  char const **_argv;
  struct stark_opt *const restrict optv;
  char const *const restrict desc;
  int _argc;
  int const optc;
  uint8_t _posc;
  bool _verified;
} stark_opts_t;

STARK_COLD bool stark_opts_init(struct stark_opts *const restrict opts);
bool stark_opts_parse(struct stark_opts *const restrict opts, int const argc,
                      char const **const argv);

#ifdef __cplusplus
}
#endif

#endif // STARK_OPTS_H
