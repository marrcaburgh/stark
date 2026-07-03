//
// stark - a C99+ utility library - stark_opts - a blazing-fast feature-full
// command-line parser
// Copyright (C) 2026 marrcaburgh
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

#ifndef STARK__OPTS_H
#define STARK__OPTS_H

#include "stark/core.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// Uncomment these to get syntax highlighting of the code in this header, the
// default (stack) parts will still be grayed out:
//
// #define STARK_OPTS_ENABLE_ENV
// #define STARK_OPTS_ENABLE_HEAP
// #define STARK_OPTS_IMPL
//
//
// Define these macros before including this header or with your build system:
//
// Define STARK_OPTS_LH_LUT_SIZE, STARK_OPTS_POS_LUT_SIZE,
// STARK_OPTS_PSC_LUT_SIZE, and/or STARK_OPTS_ENV_LUT_SIZE to change the size of
// each corresponding lookup table. Each LUT except for POS (positional) must be
// a power of two. The default sizes are listed below.
//
// Define STARK_OPTS_ENABLE_ENV to enable environment variable parsing if you're
// on an operating system that defines `environ` or `_environ` as part of the C
// standard library. See below.
//
// Define STARK_OPTS_ENABLE_HEAP to enable heap allocation for a worst-case
// sized token pool that is used to tokenize argv and slice delimited string
// arrays, and the environment if environment variable parsing is enabled.
//
// Delimited string arrays can be used with heap allocation disabled, but
// argv point to mutable strings otherwise this is undefined behavior.
//
#ifndef STARK_OPTS_LH_LUT_SIZE
#define STARK_OPTS_LH_LUT_SIZE 64
#endif // STARK_OPTS_LH_LUT_SIZE
#ifndef STARK_OPTS_POS_LUT_SIZE
#define STARK_OPTS_POS_LUT_SIZE 4
#endif // STARK_OPTS_POS_LUT_SIZE
#ifndef STARK_OPTS_PSC_LUT_SIZE
#define STARK_OPTS_PSC_LUT_SIZE 8
#endif // STARK_OPTS_PSC_LUT_SIZE
#ifndef STARK_OPTS_ENV_LUT_SIZE
#define STARK_OPTS_ENV_LUT_SIZE 16
#endif // STARK_OPTS_ENV_LUT_SIZE

typedef enum stark_opts_err {
  STARK_OPTS_ERR_UNKNOWN_OPTION = 1,
  STARK_OPTS_ERR_NO_VALUE = 2,
  STARK_OPTS_ERR_OOB = 3,
  STARK_OPTS_ERR_NAN = 4,
  STARK_OPTS_ERR_OOR = 5,
  STARK_OPTS_ERR_MISSING_REQUIRED_OPTION = 6,
  STARK_OPTS_ERR_CONFLICTING_OPTION = 7,
} stark_opts_err_t;

enum {
  /* regular types */
  STARK_OPT_TYPE_BOOLEAN = 0,
  STARK_OPT_TYPE_INT64 = 1,
  STARK_OPT_TYPE_INT32 = 2,
  STARK_OPT_TYPE_INT16 = 3,
  STARK_OPT_TYPE_INT8 = 4,
  STARK_OPT_TYPE_UINT64 = 5,
  STARK_OPT_TYPE_UINT32 = 6,
  STARK_OPT_TYPE_UINT16 = 7,
  STARK_OPT_TYPE_UINT8 = 8,
  STARK_OPT_TYPE_FLOAT64 = 9,
  STARK_OPT_TYPE_FLOAT32 = 10,
  STARK_OPT_TYPE_STRING = 11,

  /* special types */
  STARK_OPT_TYPE_SUBCOMMAND = 12,

  /* bltn types */
  STARK_OPT_TYPE_HELP = 13,
};

enum {
  STARK_OPT_MOD_REQUIRED = (1 << 0),
  STARK_OPT_MOD_HIDDEN = (1 << 1),
  STARK_OPT_MOD_POSITIONAL = (1 << 2),
  STARK_OPT_MOD_ARRAY = (1 << 3),
};

typedef struct stark_opt {
  uint16_t type : 4;
  uint16_t mods : 4;
  uint16_t cb_tag : 1;
  uint16_t _fstate : 2;
  uint16_t group : 5;                  // 2 bytes
  uint8_t arrc;                        // 1 byte
  uint8_t const arrl;                  // 1 byte
  char const delim;                    // 1 byte
  unsigned char const shorthand;       // 1 byte
  uint8_t _long_len;                   // 1 byte
  uint8_t _env_len;                    // 1 byte
  char const *const restrict longhand; // 8 bytes
  char const *const restrict env;      // 8 bytes
  void *const restrict dest;           // 8 bytes
  bool (*const assign)(char const *const restrict str,
                       void *const restrict dest, uint8_t const arrc,
                       void *restrict *const restrict vpp);
  const union {
    void (*const callback)(const void *const restrict ctx);
    bool (*const validate)(const void *const restrict val,
                           const void *const restrict ctx);
  } callback;                       // 8 bytes
  void *const restrict ctx;         // 8 bytes
  char const *const restrict usage; // 8 bytes
} stark_opt_t; // Fits into one 64-bit L1 cache line or 64 bytes of memory.

typedef struct stark_opts {
  struct stark_opt *_sh_lut[256], *_lh_lut[STARK_OPTS_LH_LUT_SIZE],
      *_psc_lut[STARK_OPTS_PSC_LUT_SIZE], *_env_lut[STARK_OPTS_ENV_LUT_SIZE],
      *_pos_lut[STARK_OPTS_POS_LUT_SIZE], **_group_table[63];
  struct stark_opt *const optv;
  char const *const restrict desc;
  void (*const err_callback)(enum stark_opts_err const errc, char const *ctx);
  char *_token_pool, *_token, **_argv;
  int _argc;
  int const optc;
  uint8_t _flags;
  uint8_t _posc;
} stark_opts_t;

STARK_COLD bool stark_opts_init(struct stark_opts *const restrict opts);
bool stark_opts_parse(struct stark_opts *const restrict opts, int const argc,
                      char **argv);
#ifdef STARK_OPTS_ENABLE_HEAP
void stark_opts_free_token_pool(struct stark_opts *const restrict opts);
void stark_opts_free_group_pools(struct stark_opts *const restrict opts);
#endif

#ifdef STARK_OPTS_IMPL

#define LUT_TYPE_LH 0
#define LUT_TYPE_PSC 1
#define LUT_TYPE_ENV 2

#define VALUE_TOKEN (1 << 0)
#define LONG_OPT (1 << 1)
#define POS_OPT (1 << 2)
#define DIRTY (1 << 3)
#define VERIFIED (1 << 4)

#include <errno.h>
#include <float.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
  NONE,
  LONGHAND,
  SHORTHAND,
  POSITIONAL,
};

STARK_COLD static void error(struct stark_opts *const restrict opts,
                             enum stark_opts_err const errc,
                             char const *const fname_octx,
                             char const *const err_ofmt, ...) {
  if (opts != NULL) {
    if (opts->err_callback != NULL) {
      opts->err_callback(errc, err_ofmt);

      return;
    }

    switch (errc) {
    case STARK_OPTS_ERR_UNKNOWN_OPTION:
      fprintf(stderr, "unknown option: '%s%s'",
              opts->_flags & POS_OPT    ? ""
              : opts->_flags & LONG_OPT ? "--"
                                        : "-",
              err_ofmt);

      break;
    case STARK_OPTS_ERR_NO_VALUE:
      fprintf(stderr, "no value for option (expected: %s): '%s%s'", fname_octx,
              opts->_flags & POS_OPT    ? ""
              : opts->_flags & LONG_OPT ? "--"
                                        : "-",
              err_ofmt);

      break;
    case STARK_OPTS_ERR_OOB:
      fprintf(stderr, "out of bounds: '%s'", err_ofmt);

      break;
    case STARK_OPTS_ERR_NAN:
      fprintf(stderr, "not a number: '%s'", err_ofmt);

      break;
    case STARK_OPTS_ERR_OOR:
      fprintf(stderr, "out of range (expected: %s): '%s'", fname_octx,
              err_ofmt);

      break;
    case STARK_OPTS_ERR_MISSING_REQUIRED_OPTION:
      fprintf(stderr, "missing required option: '%s'", err_ofmt);

      break;
    case STARK_OPTS_ERR_CONFLICTING_OPTION:
      fprintf(stderr, "conflicting option %s", err_ofmt);

      break;
    default:
      return;
    }
  } else {
    fprintf(stderr, "stark_opts error: %s(): ", fname_octx);

    va_list ap;

    va_start(ap, err_ofmt);
    vfprintf(stderr, err_ofmt, ap);
    va_end(ap);
  }

  fprintf(stderr, "\n");
}

STARK_NOINLINE static bool
run_subcommand(struct stark_opts *const restrict opts,
               struct stark_opt *const restrict opt) {
  return stark_opts_parse((struct stark_opts *)opt->ctx, opts->_argc,
                          opts->_argv);
}

STARK_COLD STARK_ALWAYS_INLINE static inline void
help(STARK_UNUSED struct stark_opts *const restrict opts) {
  // TODO: implement help generator with declarative configuration for
  // formatting
}

STARK_ALWAYS_INLINE static inline bool
assign_opt(struct stark_opts *const restrict opts,
           struct stark_opt *const restrict opt) {
  if (opt->callback.callback != NULL && opt->cb_tag == 0) {
    opt->callback.callback(opt->ctx);

    return true;
  } else if (opt->assign == NULL) {
    if (opt->type == STARK_OPT_TYPE_HELP) {
      help(opts);

      return true;
    } else if (opt->type == STARK_OPT_TYPE_SUBCOMMAND) {
      if (STARK_EXPECT_FALSE(!run_subcommand(opts, opt))) {
        return false;
      }

      opts->_argc = 0;

      return true;
    }
  }

  void *vp = NULL;
  char *str = NULL, *delim = NULL, *rcp;

assign_opt_carr:
  if (STARK_EXPECT_FALSE(opt->mods & STARK_OPT_MOD_ARRAY &&
                         opt->arrc > opt->arrl)) {
    error(opts, STARK_OPTS_ERR_OOB, NULL, opts->_token);

    return false;
  }

  if (opt->type == STARK_OPT_TYPE_BOOLEAN) {
    ((bool *)opt->dest)[opt->arrc] = true;

    goto assign_opt_skip_val;
  } else if (opt->mods & STARK_OPT_MOD_POSITIONAL) {
    goto assign_opt_skip_vfind_os;
  } else if (opts->_flags & VALUE_TOKEN) {
    opts->_flags &= ~VALUE_TOKEN;

    goto assign_opt_skip_vfind_os;
  } else if (opts->_argc > 1) {
  assign_opt_retry_narg:
    opts->_argc--;
    opts->_argv++;

    if (STARK_EXPECT_FALSE(opts->_argc == 0)) {
      return false;
    } else if (STARK_EXPECT_FALSE(opts->_argv == NULL ||
                                  *(opts->_argv)[0] == '\0')) {
      goto assign_opt_retry_narg;
    } else {
#ifdef STARK_OPTS_ENABLE_HEAP
      strcpy(opts->_token, *opts->_argv);
#else
      opts->_token = *opts->_argv;
#endif // STARK_OPTS_ENABLE_HEAP

      goto assign_opt_skip_vfind_os;
    }
  }

  char const *ot;

  switch (opt->type) {
  case STARK_OPT_TYPE_INT64:
  case STARK_OPT_TYPE_INT32:
  case STARK_OPT_TYPE_INT16:
  case STARK_OPT_TYPE_INT8:
    ot = "a non-decimal number";

    break;
  case STARK_OPT_TYPE_UINT64:
  case STARK_OPT_TYPE_UINT32:
  case STARK_OPT_TYPE_UINT16:
  case STARK_OPT_TYPE_UINT8:
    ot = "a positive non-decimal number";

    break;
  case STARK_OPT_TYPE_FLOAT64:
  case STARK_OPT_TYPE_FLOAT32:
    ot = "a decimal number";
    break;
  case STARK_OPT_TYPE_STRING:
    ot = "text";
    break;
  }

  error(opts, STARK_OPTS_ERR_NO_VALUE, ot, opts->_token);

  return false;

assign_opt_skip_vfind_os:
  str = opts->_token;

  if (opt->assign != NULL) {
    if (STARK_EXPECT_FALSE(!opt->assign(str, opt->dest, opt->arrc, &vp))) {
      return false;
    }

    goto assign_opt_skip_bltn_os;
  } else if (opt->type == STARK_OPT_TYPE_STRING) {
    if (opt->mods & STARK_OPT_MOD_ARRAY) {
      char *tp = str;

    assign_opt_fnext_delim:
      if ((delim = strchr(tp, opt->delim)) != NULL && *delim != '\0') {
        bool esc = false;

        for (tp = delim - 1; tp >= str && *tp == '\\'; tp--, esc = !esc)
          ;

        if (esc) {
          tp = memmove(str + (delim - tp) - 1, str, tp - str + 1);

          if (tp == NULL) {
            return false;
          }

          str = tp;
          tp = delim + 1;

          goto assign_opt_fnext_delim;
        }
      }
    }

    ((char const **)opt->dest)[opt->arrc] = str;
    vp = &((char const **)opt->dest)[opt->arrc];

#ifdef STARK_OPTS_ENABLE_HEAP
    opts->_token = strchr(str, '\0') + 1;
#endif

    goto assign_opt_skip_bltn_os;
  }

  char *endptr = NULL, rs[52];
  union {
    int64_t i;
    uint64_t ui;
    float f;
    double d;
  } val;
  errno = 0;

  if ((opt->type >= STARK_OPT_TYPE_INT64) &&
      (opt->type <= STARK_OPT_TYPE_INT8)) {
    val.i = strtoll(str, &endptr, 10);
  } else if ((opt->type >= STARK_OPT_TYPE_UINT64) &&
             (opt->type <= STARK_OPT_TYPE_UINT8)) {
    val.ui = strtoull(str, &endptr, 10);
  } else if (opt->type == STARK_OPT_TYPE_FLOAT64) {
    val.d = strtod(str, &endptr);
  } else if (opt->type == STARK_OPT_TYPE_FLOAT32) {
    val.f = strtof(str, &endptr);
  }

  if ((opt->mods & STARK_OPT_MOD_ARRAY) && *endptr == opt->delim) {
    opts->_token = (delim = endptr) + 1;
    opts->_flags |= VALUE_TOKEN;
  } else if (STARK_EXPECT_FALSE(endptr == str) || *endptr != '\0') {
    error(opts, STARK_OPTS_ERR_NAN, NULL, str);

    return false;
  }

  switch (opt->type) {
  case STARK_OPT_TYPE_INT64:
    if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      sprintf(rs, "'%ld' to '%ld'", INT64_MIN, INT64_MAX);

      break;
    }

    ((int64_t *)opt->dest)[opt->arrc] = val.i;
    vp = &((int64_t *)opt->dest)[opt->arrc];

    goto assign_opt_skip_bltn_os;
  case STARK_OPT_TYPE_INT32:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.i > INT32_MAX ||
                           val.i < INT32_MIN)) {
      sprintf(rs, "'%d' to '%d'", INT32_MIN, INT32_MAX);

      break;
    }

    ((int32_t *)opt->dest)[opt->arrc] = (int32_t)val.i;
    vp = &((int32_t *)opt->dest)[opt->arrc];

    goto assign_opt_skip_bltn_os;
  case STARK_OPT_TYPE_INT16:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.i > INT16_MAX ||
                           val.i < INT16_MIN)) {
      sprintf(rs, "'%hd' to '%hd'", INT16_MIN, INT16_MAX);

      break;
    }

    ((int16_t *)opt->dest)[opt->arrc] = (int16_t)val.i;
    vp = &((int16_t *)opt->dest)[opt->arrc];

    goto assign_opt_skip_bltn_os;
  case STARK_OPT_TYPE_INT8:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.i > INT8_MAX ||
                           val.i < INT8_MIN)) {
      sprintf(rs, "'%hhd' to '%hhd'", INT8_MIN, INT8_MAX);

      break;
    }

    ((int8_t *)opt->dest)[opt->arrc] = (int8_t)val.i;
    vp = &((int8_t *)opt->dest)[opt->arrc];

    goto assign_opt_skip_bltn_os;
  case STARK_OPT_TYPE_UINT64:
    for (rcp = str; *rcp == ' ' || *rcp == '\t'; rcp++)
      ;

    if (STARK_EXPECT_FALSE(errno == ERANGE || rcp[0] == '-')) {
      sprintf(rs, "'%lu' to '%lu'", 0UL, UINT64_MAX);

      break;
    }

    ((uint64_t *)opt->dest)[opt->arrc] = val.ui;
    vp = &((uint64_t *)opt->dest)[opt->arrc];

    goto assign_opt_skip_bltn_os;
  case STARK_OPT_TYPE_UINT32:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.ui > UINT32_MAX)) {
      sprintf(rs, "'%u' to '%u'", 0, UINT32_MAX);

      break;
    }

    ((uint32_t *)opt->dest)[opt->arrc] = (uint32_t)val.ui;
    vp = &((uint32_t *)opt->dest)[opt->arrc];

    goto assign_opt_skip_bltn_os;
  case STARK_OPT_TYPE_UINT16:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.ui > UINT16_MAX)) {
      sprintf(rs, "'%hu' to '%hu'", 0, UINT16_MAX);

      break;
    }

    ((uint16_t *)opt->dest)[opt->arrc] = (uint16_t)val.ui;
    vp = &((uint16_t *)opt->dest)[opt->arrc];

    goto assign_opt_skip_bltn_os;
  case STARK_OPT_TYPE_UINT8:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.ui > UINT8_MAX)) {
      sprintf(rs, "'%hhu' to '%hhu'", 0, UINT8_MAX);

      break;
    }

    ((uint8_t *)opt->dest)[opt->arrc] = (uint8_t)val.ui;
    vp = &((uint8_t *)opt->dest)[opt->arrc];

    goto assign_opt_skip_bltn_os;
  case STARK_OPT_TYPE_FLOAT64:
    if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      sprintf(rs, "'%g' to '%g'", -DBL_MAX, DBL_MAX);

      break;
    }

    ((double *)opt->dest)[opt->arrc] = val.d;
    vp = &((double *)opt->dest)[opt->arrc];

    goto assign_opt_skip_bltn_os;
  case STARK_OPT_TYPE_FLOAT32:
    if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      sprintf(rs, "'%g' to '%g'", -FLT_MAX, FLT_MAX);

      break;
    }

    ((float *)opt->dest)[opt->arrc] = val.f;
    vp = &((float *)opt->dest)[opt->arrc];

    goto assign_opt_skip_bltn_os;
  }

  error(opts, STARK_OPTS_ERR_OOR, rs, str);

  return false;

assign_opt_skip_bltn_os:
  if (vp != NULL && opt->callback.validate != NULL && opt->cb_tag == 1 &&
      STARK_EXPECT_FALSE(!opt->callback.validate(vp, opt->ctx))) {
    return false;
  }

assign_opt_skip_val:
  if (opt->mods & STARK_OPT_MOD_ARRAY) {
    opt->arrc++;

    if (delim != NULL) {
      if (opt->type == STARK_OPT_TYPE_STRING) {
        *delim = '\0';
      }

      opts->_token = delim + 1;
      opts->_flags |= VALUE_TOKEN;
      delim = NULL;

      goto assign_opt_carr;
    }
  }

  return true;
}

STARK_ALWAYS_INLINE static inline size_t hashn(char const *restrict str,
                                               size_t const n) {
  for (size_t h = sizeof(size_t) == 8 ? 14695981039346656037u : 2166136261u,
              i = 0;
       ; i++, h ^= (unsigned char)*str++,
              h *= sizeof(size_t) == 8 ? 1099511628211u : 16777619u) {
    if (i == n) {
      return h;
    }
  }
}

STARK_COLD STARK_ALWAYS_INLINE static inline bool
lut_insert(struct stark_opt *const restrict opt, struct stark_opt **lut,
           size_t const lut_size, uint8_t const type) {
  uint8_t *lp;
  char const *ot, *ls, *os;
  size_t i, probes = 0, len = 0;

  switch (type) {
  case 0:
    ot = "longhand";
    ls = "STARK_OPTS_LH_LUT_SIZE";
    os = opt->longhand;
    lp = &opt->_long_len;
    break;
  case 1:
    ot = "positional subcommand";
    ls = "STARK_OPTS_PSC_LUT_SIZE";
    os = opt->longhand;
    lp = &opt->_long_len;
    break;
  case 2:
    ot = "environment";
    ls = "STARK_OPTS_ENV_LUT_SIZE";
    os = opt->env;
    lp = &opt->_env_len;
    break;
  }

  if ((len = strlen(os)) > 255) {
    error(NULL, 0, "lut_insert", "%s option '%s' length is greater than 255",
          ot, os);

    return false;
  } else if (strchr(os, '=') != NULL) {
    error(NULL, 0, "lut_insert", "%s option '%s' has an '='", ot, os);

    return false;
  }

  *lp = len;
  i = hashn(os, *lp) & (lut_size - 1);

  for (struct stark_opt const *prev = lut[i]; prev != NULL;
       i = (i + 1) & (lut_size - 1), prev = lut[i]) {
    char const *pos = type == 2 ? prev->env : prev->longhand;

    if (pos != NULL && strcmp(pos, os) == 0) {
      goto lut_insert_duplicate;
    }

    if (++probes == lut_size) {
      error(NULL, 0, "lut_insert",
            "%s option lookup table is full; define "
            "%s before "
            "inclusion with a greater limit or remove options",
            ot, ls);

      return false;
    }

    continue;

  lut_insert_duplicate:
    error(NULL, 0, "lut_insert", "duplicate %s option '%s'", ot, os);

    return false;
  }

  lut[i] = opt;

  return true;
}

STARK_COLD bool stark_opts_init(struct stark_opts *const restrict opts) {
  bool ok = true, vp = false;

  if (opts == NULL) {
    error(NULL, 0, "stark_opts_init", "opts cannot be NULL");

    return false;
  } else if (opts->optv == NULL) {
    error(NULL, 0, "stark_opts_init", "optv cannot be NULL");

    return false;
  } else if (opts->optc <= 0) {
    error(NULL, 0, "stark_opts_init", "optc must be greater than zero");

    return false;
  } else if (opts->_flags & VERIFIED) {
    return true;
  }

  if ((STARK_OPTS_LH_LUT_SIZE == 0) ||
      (!((STARK_OPTS_LH_LUT_SIZE & (STARK_OPTS_LH_LUT_SIZE - 1)) == 0))) {
    error(NULL, 0, "stark_opts_init",
          "STARK_OPTS_LH_LUT_SIZE must be a power of two");

    return false;
  }

  if ((STARK_OPTS_PSC_LUT_SIZE == 0) ||
      (!((STARK_OPTS_PSC_LUT_SIZE & (STARK_OPTS_PSC_LUT_SIZE - 1)) == 0))) {
    error(NULL, 0, "stark_opts_init",
          "STARK_OPTS_PSC_LUT_SIZE must be a power of two");

    return false;
  }

#ifdef STARK_OPTS_ENABLE_ENV
  if ((STARK_OPTS_ENV_LUT_SIZE == 0) ||
      (!((STARK_OPTS_ENV_LUT_SIZE & (STARK_OPTS_ENV_LUT_SIZE - 1)) == 0))) {
    error(NULL, 0, "stark_opts_init",
          "STARK_OPTS_ENV_LUT_SIZE must be a power of two");

    return false;
  }
#endif

  int li = 0;
stark_opts_init_loop:
  if (li == opts->optc) {
    goto stark_opts_init_loope;
  }

  stark_opt_t *const restrict op = &opts->optv[li];

#ifdef STARK_OPTS_ENABLE_HEAP
  if (op->group != 0) {
    size_t c = 1;

    c += opts->_group_table[op->group - 1] != NULL
             ? *((size_t *)opts->_group_table[op->group - 1])
             : 1;

    struct stark_opt **tp =
        realloc(opts->_group_table[op->group - 1],
                sizeof(size_t) + c * sizeof(struct stark_opt **));

    if (tp == NULL) {
      error(NULL, 0, "stark_opts_init", "reallocation for group pool failed");

      return false;
    }

    *((size_t *)(opts->_group_table[op->group - 1] = tp)) = c;
    opts->_group_table[op->group - 1][c - 1] = op;
    opts->_group_table[op->group - 1][c] = NULL;
  }
#endif

  if (op->mods & STARK_OPT_MOD_ARRAY) {
    if (op->arrl <= 1) {
      error(NULL, 0, "stark_opts_init",
            "array options must have an arrl that is greater than one");

      ok = false;
    }

    switch (op->delim) {
    case ',':
    case ':':
    case ';':
      if (op->type == STARK_OPT_TYPE_BOOLEAN) {
        error(NULL, 0, "stark_opts_init",
              "boolean options cannot have delimiters");

        ok = false;
      }
      break;
    default:
      if (op->type != STARK_OPT_TYPE_BOOLEAN
#ifndef STARK_OPTS_ENABLE_HEAP
          && op->type != STARK_OPT_TYPE_STRING
#endif // STARK_OPTS_ENABLE_HEAP
      ) {
        error(NULL, 0, "stark_opts_init",
              "missing delimiter for array option (expected one of: ',', ':', "
              "';')");

        ok = false;
      }

      break;
    }
  }

  if (op->assign != NULL) {
    if (op->dest == NULL) {
      error(NULL, 0, "stark_opts_init", "option missing destination pointer");

      ok = false;
    }

    goto stark_opts_init_skip_bltn;
  } else if (op->callback.callback != NULL && op->cb_tag == 0) {
    goto stark_opts_init_skip_bltn;
  }

  switch (op->type) {
  case STARK_OPT_TYPE_STRING:
  case STARK_OPT_TYPE_INT64:
  case STARK_OPT_TYPE_INT32:
  case STARK_OPT_TYPE_INT16:
  case STARK_OPT_TYPE_INT8:
  case STARK_OPT_TYPE_UINT64:
  case STARK_OPT_TYPE_UINT32:
  case STARK_OPT_TYPE_UINT16:
  case STARK_OPT_TYPE_UINT8:
  case STARK_OPT_TYPE_FLOAT64:
  case STARK_OPT_TYPE_FLOAT32:
  case STARK_OPT_TYPE_BOOLEAN:
    if (op->dest == NULL) {
      error(NULL, 0, "stark_opts_init", "option missing destination pointer");

      ok = false;
    }

    break;
  case STARK_OPT_TYPE_SUBCOMMAND:
    if (op->ctx == NULL) {
      error(NULL, 0, "stark_opts_init", "subcommand option missing context");

      ok = false;
    }

    break;
  case STARK_OPT_TYPE_HELP:
    break;
  }

stark_opts_init_skip_bltn:
  if (op->mods & STARK_OPT_MOD_POSITIONAL) {
    if (op->type == STARK_OPT_TYPE_SUBCOMMAND) {
      if (op->longhand == NULL) {
        error(NULL, 0, "stark_opts_init",
              "positional subcommand options must have a longhand");

        ok = false;
      } else {
        op->_long_len = strlen(op->longhand);
        ok &= lut_insert(op, opts->_psc_lut, STARK_OPTS_PSC_LUT_SIZE,
                         LUT_TYPE_PSC);
      }

      goto stark_opts_init_skip_rpos;
    } else if (op->type == STARK_OPT_TYPE_BOOLEAN) {
      error(NULL, 0, "stark_opts_init",
            "positional option modifier cannot be combined with boolean "
            "type");

      ok = false;
    }

    if (opts->_posc == STARK_OPTS_POS_LUT_SIZE) {
      error(NULL, 0, "stark_opts_init",
            "positional option count exceeds limit; define "
            "STARK_OPTS_POS_LUT_SIZE before inclusion with a greater limit or "
            "remove "
            "options");

      ok = false;
    } else {
      if (vp) {
        error(NULL, 0, "stark_opts_init",
              "variadic positional options must be the last positional");

        ok = false;
      }

      vp = (op->mods & STARK_OPT_MOD_ARRAY);
      opts->_pos_lut[opts->_posc++] = op;
    }

  stark_opts_init_skip_rpos:
    goto stark_opts_init_skip_onc;
  } else if (op->shorthand == '\0' && op->longhand == NULL) {
    error(NULL, 0, "stark_opts_init",
          "non-positional options must have either a shorthand "
          "or longhand");

    ok = false;
  }

  if (op->shorthand == '\0') {
    goto stark_opts_init_skip_sh;
  }

  if (opts->_sh_lut[op->shorthand] == NULL) {
    opts->_sh_lut[op->shorthand] = op;
  } else {
    error(NULL, 0, "stark_opts_init", "duplicate shorthand option '%c'",
          op->shorthand);

    ok = false;
  }

stark_opts_init_skip_sh:
  if (op->longhand != NULL) {
    ok &= lut_insert(op, opts->_lh_lut, STARK_OPTS_LH_LUT_SIZE, LUT_TYPE_LH);
  }

#ifdef STARK_OPTS_ENABLE_ENV
  if (op->env != NULL) {
    ok &= lut_insert(op, opts->_env_lut, STARK_OPTS_ENV_LUT_SIZE, LUT_TYPE_ENV);
  }
#endif

stark_opts_init_skip_onc:
  li++;

  goto stark_opts_init_loop;

stark_opts_init_loope:
  return opts->_flags |= (ok ? VERIFIED : 0);
}

STARK_ALWAYS_INLINE STARK_FLATTEN static inline struct stark_opt *
probe(struct stark_opts *const restrict opts,
      struct stark_opt *const restrict *lut, size_t const lut_size,
      uint8_t const type) {
  char *restrict eq = opts->_token;

  for (; eq[0] != '=' && eq[0] != '\0'; eq++)
    ;

  if (STARK_EXPECT_FALSE(eq[0] == '\0' && type == LUT_TYPE_ENV)) {
    return NULL;
  }

  for (size_t probes = 0, tkn_len = (size_t)(eq - opts->_token),
              i = hashn(opts->_token, tkn_len) & (lut_size - 1);
       probes != lut_size; i = (i + 1) & (lut_size - 1), probes++) {
    struct stark_opt *op;

    if (STARK_EXPECT_FALSE((op = lut[i]) == NULL)) {
      break;
    }

    char const *const os = type == LUT_TYPE_ENV ? op->env : op->longhand;
    uint8_t *const lp = type == LUT_TYPE_ENV ? &op->_env_len : &op->_long_len;

    if (STARK_EXPECT_TRUE(*lp == tkn_len &&
                          memcmp(os, opts->_token, tkn_len) == 0)) {
      if (type != LUT_TYPE_PSC && eq[0] != '\0') {
        if (eq[1] == '\0') {
          eq[0] = '\0';
        } else {
          opts->_token = eq + 1;
        }
      } else {
        opts->_flags &= ~VALUE_TOKEN;
      }

      op->_fstate = LONGHAND;

      return op;
    }
  }

  return NULL;
}

bool stark_opts_parse(struct stark_opts *const restrict opts, int const argc,
                      char **argv) {
  if (STARK_EXPECT_FALSE(opts == NULL)) {
    error(NULL, 0, "stark_opts_parse", "opts cannot be NULL");

    return false;
  } else if (STARK_EXPECT_FALSE(argv == NULL)) {
    error(NULL, 0, "stark_opts_parse", "argv cannot be NULL");

    return false;
  } else if (STARK_EXPECT_FALSE(argc <= 0)) {
    error(NULL, 0, "stark_opts_parse", "argc must be greater than zero");

    return false;
  } else if (STARK_EXPECT_FALSE(!(opts->_flags & VERIFIED))) {
    error(NULL, 0, "stark_opts_parse",
          "not verified; did you forget "
          "'stark_opts_init()'?");

    return false;
  }
#ifdef STARK_OPTS_ENABLE_HEAP
  else if (STARK_EXPECT_FALSE(((opts->_flags & DIRTY) &&
                               opts->_token_pool != NULL &&
                               opts->_token != NULL))) {
    error(NULL, 0, "stark_opts_parse",
          "dirty; did you forget 'stark_opts_free_token_pool()' or "
          "'stark_opts_free_group_pools()'?");

    return false;
  }
#endif

  struct stark_opt *op;

  for (int i = 0; i < opts->optc; i++) {
    op = &opts->optv[i];
    op->_fstate = NONE;
    op->arrc = 0;
  }

#ifdef STARK_OPTS_ENABLE_HEAP
  opts->_token_pool = malloc(
#ifdef ARG_MAX
      ARG_MAX
#else // ARG_MAX
#ifdef _WIN32
      UINT16_MAX
#else  // _WIN32
      1024 * 1024 * 6
#endif // _WIN32
#endif // ARG_MAX
  );

  if (opts->_token_pool == NULL) {
    error(NULL, 0, "stark_opts_parse", "allocation failed for token pool");

    return false;
  }

  opts->_token = opts->_token_pool;

#ifdef STARK_OPTS_ENABLE_ENV
  char **envp;
#ifdef _WIN32
  extern char **_environ;
  envp = _environ;
#else // _WIN32
#ifdef __unix__
  extern char **environ;
  envp = environ;
#else  // __unix__
  envp = NULL;
#endif // __unix__
#endif // _WIN32

  if (STARK_EXPECT_FALSE(envp == NULL)) {
    error(NULL, 0, "stark_opts_parse",
          "unsupported operating system for environment variable parsing; "
          "remove STARK_OPTS_ENABLE_ENV");

    return false;
  }

  for (; *envp != NULL; envp++, opts->_flags |= VALUE_TOKEN) {
    strcpy(opts->_token, *envp);

    if (STARK_EXPECT_FALSE(
            (op = probe(opts, opts->_env_lut, STARK_OPTS_ENV_LUT_SIZE,
                        LUT_TYPE_ENV)) != NULL)) {

      if (STARK_EXPECT_FALSE(!assign_opt(opts, op))) {
        return false;
      }
    }
  }
#endif // STARK_OPTS_ENABLE_ENV
#endif // STARK_OPTS_ENABLE_HEAP

  size_t pos_idx = 0;
  bool eoo = false;

  for (opts->_argc = argc - 1, opts->_argv = argv + 1, op = NULL,
      opts->_flags |= VALUE_TOKEN;
       opts->_argc > 0;
       opts->_argc--, opts->_argv++, op = NULL, opts->_flags |= VALUE_TOKEN) {
    if (STARK_EXPECT_FALSE((*opts->_argv) == NULL ||
                           (*opts->_argv)[0] == '\0')) {
      continue;
    }

    if (eoo || (*opts->_argv)[0] != '-' || (*opts->_argv)[1] == '\0') {
#ifdef STARK_OPTS_ENABLE_HEAP
      strcpy(opts->_token, *opts->_argv);
#else
      opts->_token = *opts->_argv;
#endif
      op = !eoo ? probe(opts, opts->_psc_lut, STARK_OPTS_PSC_LUT_SIZE,
                        LUT_TYPE_PSC)
                : NULL;

      if (STARK_EXPECT_FALSE(op == NULL)) {
        if ((!eoo) & (pos_idx == opts->_posc)) {
          goto stark_opts_parse_uopt;
        } else {
          op = opts->_pos_lut[pos_idx];
        }
      } else {
        op->_fstate = POSITIONAL;
      }

      opts->_flags |= POS_OPT;

      if (STARK_EXPECT_FALSE(!assign_opt(opts, op))) {
        return false;
      }

      if (op->arrc == op->arrl) {
        pos_idx++;
      }

      continue;
    }

    opts->_flags &= ~POS_OPT;

    if ((*opts->_argv)[1] == '-') {
      goto stark_opts_parse_lh;
    }

    bool mop;

#ifdef STARK_OPTS_ENABLE_HEAP
    strcpy(opts->_token, &(*opts->_argv)[1]);
#else
    opts->_token = &(*opts->_argv)[1];
#endif

    while (opts->_flags & VALUE_TOKEN) {
      if (STARK_EXPECT_FALSE(
              (op = opts->_sh_lut[(unsigned char)opts->_token[0]]) == NULL)) {
        goto stark_opts_parse_uopt;
      }

      if ((mop = opts->_token[1] != '\0')) {
        opts->_token = opts->_token + 1;
      } else {
        opts->_flags &= ~VALUE_TOKEN;
      }

      op->_fstate = SHORTHAND;
      opts->_flags &= ~LONG_OPT;

      if (STARK_EXPECT_FALSE(!assign_opt(opts, op))) {
        return false;
      }

      if (!mop) {
        break;
      }
    }

    continue;

  stark_opts_parse_lh:
    if ((*opts->_argv)[2] == '\0') {
      eoo = true;
      continue;
    }

#ifdef STARK_OPTS_ENABLE_HEAP
    strcpy(opts->_token, &(*opts->_argv)[2]);
#else
    opts->_token = &(*opts->_argv)[2];
#endif

    if (STARK_EXPECT_FALSE((op = probe(opts, opts->_lh_lut,
                                       STARK_OPTS_LH_LUT_SIZE, LUT_TYPE_LH)) ==
                           NULL)) {
      goto stark_opts_parse_uopt;
    }

    opts->_flags |= LONG_OPT;

    if (STARK_EXPECT_FALSE(!assign_opt(opts, op))) {
      return false;
    }

    continue;

  stark_opts_parse_uopt:
    error(opts, STARK_OPTS_ERR_UNKNOWN_OPTION, NULL, opts->_token);

    return false;
  }

  for (int i = 0; i < opts->optc; i++) {
    op = &opts->optv[i];

    if (op->_fstate != NONE) {
      if (op->group != 0) {
        for (struct stark_opt **oop = &opts->_group_table[op->group - 1][1];
             *oop != NULL; oop++) {
          if (*oop == op) {
            continue;
          }

          char buf[UINT8_MAX * 2 + 28];
          int off = 0;

          switch ((*oop)->_fstate) {
          case NONE:
            continue;
          case SHORTHAND:
            off = sprintf(buf, "'-%c' with option '-", (*oop)->shorthand);
            break;
          case LONGHAND:
            off = sprintf(buf, "'--%s' with option '--", (*oop)->longhand);
            break;
          case POSITIONAL:
            off = sprintf(buf, "'%s' with option '", (*oop)->longhand);
            break;
          }

          switch (op->_fstate) {
          case SHORTHAND:
            sprintf(buf + off, "%c'", op->shorthand);
            break;
          case POSITIONAL:
          case LONGHAND:
            sprintf(buf + off, "%s'", op->longhand);
            break;
          }

          error(opts, STARK_OPTS_ERR_CONFLICTING_OPTION, NULL, buf);

          return false;
        }
      }
    } else if (op->mods & STARK_OPT_MOD_REQUIRED) {
      char buf[UINT8_MAX + 7];

      sprintf(buf,
              (op->mods & STARK_OPT_MOD_POSITIONAL) &&
                      (op->type == STARK_OPT_TYPE_SUBCOMMAND)
                  ? "%s"
              : op->longhand != NULL ? "--%s"
                                     : "-%s",
              op->longhand != NULL ? op->longhand
                                   : (char[]){op->shorthand, '\0'});

      error(opts, STARK_OPTS_ERR_MISSING_REQUIRED_OPTION, NULL, buf);

      return false;
    }
  }

  return true;
}

#ifdef STARK_OPTS_ENABLE_HEAP
void stark_opts_free_token_pool(struct stark_opts *const restrict opts) {
  free(opts->_token_pool);
  opts->_token_pool = NULL;
  opts->_token = NULL;
  opts->_flags &= ~DIRTY;
}

void stark_opts_free_group_pools(struct stark_opts *const restrict opts) {
  for (int i = 0; i < 63; i++) {
    free(opts->_group_table[i]);
    opts->_group_table[i] = NULL;
  }
}
#endif

#endif // STARK_OPTS_IMPL

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // STARK__OPTS_H
