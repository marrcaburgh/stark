//
// stark - a C99+ utility library - stark_cli_opts - a blazing-fast feature-full
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

#ifndef STARK_INTERNAL_CLI_OPTS_H
#define STARK_INTERNAL_CLI_OPTS_H

//
// Uncomment these to get syntax highlighting of the majority of the code in
// this header, the default (stack) parts will still be grayed out:
//
// #define STARK_CLI_OPTS_ENABLE_ENV
// #define STARK_CLI_OPTS_ENABLE_HEAP
//

#ifdef STARK_HASH_TABLE_ENABLE_HEAP
#undef STARK_HASH_TABLE_ENABLE_HEAP
#define HASH_TABLE_RESTORE_HEAP
#endif // STARK_HASH_TABLE_ENABLE_HEAP

#define STARK_HASH_TABLE_DISABLE_ERROR_PRINTING
#define STARK_HASH_TABLE_IMPL
#include "stark/internal/hash_table.h"

#ifdef HASH_TABLE_RESTORE_HEAP
#define STARK_HASH_TABLE_ENABLE_HEAP
#undef HASH_TABLE_RESTORE_HEAP
#endif // HASH_TABLE_RESTORE_HEAP

#include <errno.h>
#include <float.h>
#include <limits.h>
#ifdef __linux__
#include <linux/limits.h>
#endif // __linux__

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LUT_TYPE_LH 0
#define LUT_TYPE_PSC 1
#define LUT_TYPE_ENV 2

#define VALUE_TOKEN (1u << 0)
#define LONG_OPT (1u << 1)
#define POS_OPT (1u << 2)
#define DIRTY (1u << 3)
#define VERIFIED (1u << 4)

enum {
  NONE,
  LONGHAND,
  SHORTHAND,
  POSITIONAL,
};

STARK_COLD static void error(struct stark_cli_opts *const restrict opts,
                             enum stark_cli_opts_err const errc,
                             char const *const fname_octx,
                             char const *const err_ofmt, ...) {
  if (opts != NULL) {
    if (opts->err_callback != NULL) {
      opts->err_callback(errc, err_ofmt);

      return;
    }

    switch (errc) {
    case STARK_CLI_OPTS_ERR_UNKNOWN_OPTION:
      fprintf(stderr, "unknown option: '%s%s'",
              opts->_flags & POS_OPT    ? ""
              : opts->_flags & LONG_OPT ? "--"
                                        : "-",
              err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_NO_VALUE:
      fprintf(stderr, "no value for option (expected: %s): '%s%s'", fname_octx,
              opts->_flags & POS_OPT    ? ""
              : opts->_flags & LONG_OPT ? "--"
                                        : "-",
              err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_OOB:
      fprintf(stderr, "out of bounds: '%s'", err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_NAN:
      fprintf(stderr, "not a number: '%s'", err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_OOR:
      fprintf(stderr, "out of range (expected: %s): '%s'", fname_octx,
              err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_MISSING_REQUIRED_OPTION:
      fprintf(stderr, "missing required option: '%s'", err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_CONFLICTING_OPTION:
      fprintf(stderr, "conflicting option %s", err_ofmt);

      break;
    default:
      return;
    }
  } else {
    fprintf(stderr, "stark_cli_opts error: %s(): ", fname_octx);

    va_list ap;

    va_start(ap, err_ofmt);
    vfprintf(stderr, err_ofmt, ap);
    va_end(ap);
  }

  fprintf(stderr, "\n");
}

STARK_NOINLINE static bool
run_subcommand(struct stark_cli_opts *const restrict opts,
               struct stark_opt *const restrict opt) {
  return stark_cli_opts_parse((struct stark_cli_opts *)opt->ctx, opts->_argc,
                              opts->_argv);
}

STARK_COLD STARK_ALWAYS_INLINE static inline void
help(STARK_UNUSED struct stark_cli_opts *const restrict opts) {
  // TODO: implement help generator with declarative configuration for
  // formatting
}

STARK_ALWAYS_INLINE static inline bool
assign_opt(struct stark_cli_opts *const restrict opts,
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
                         !(opt->arrc < opt->arrl))) {
    error(opts, STARK_CLI_OPTS_ERR_OOB, NULL, opts->_token);

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
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
      strcpy(opts->_token, *opts->_argv);
#else
      opts->_token = *opts->_argv;
#endif // STARK_CLI_OPTS_ENABLE_HEAP

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

  error(opts, STARK_CLI_OPTS_ERR_NO_VALUE, ot, opts->_token);

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

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
    opts->_token = strchr(str, '\0') + 1;
#endif // STARK_CLI_OPTS_ENABLE_HEAP

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
    val.i = strtoll(str, &endptr, 0);
  } else if ((opt->type >= STARK_OPT_TYPE_UINT64) &&
             (opt->type <= STARK_OPT_TYPE_UINT8)) {
    val.ui = strtoull(str, &endptr, 0);
  } else if (opt->type == STARK_OPT_TYPE_FLOAT64) {
    val.d = strtod(str, &endptr);
  } else if (opt->type == STARK_OPT_TYPE_FLOAT32) {
    val.f = strtof(str, &endptr);
  }

  if ((opt->mods & STARK_OPT_MOD_ARRAY) && *endptr == opt->delim) {
    opts->_token = (delim = endptr) + 1;
    opts->_flags |= VALUE_TOKEN;
  } else if (STARK_EXPECT_FALSE(endptr == str) || *endptr != '\0') {
    error(opts, STARK_CLI_OPTS_ERR_NAN, NULL, str);

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

  error(opts, STARK_CLI_OPTS_ERR_OOR, rs, str);

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

STARK_COLD STARK_ALWAYS_INLINE static inline bool
lut_insert(struct stark_opt *const restrict opt, struct stark_hash_table *lut,
           uint8_t const type) {
  char const *ot, *ls, *os;
  size_t len = 0;

  switch (type) {
  case 0:
    ot = "longhand";
    ls = "STARK_CLI_OPTS_LH_LUT_SIZE";
    os = opt->longhand;
    break;
  case 1:
    ot = "positional subcommand";
    ls = "STARK_CLI_OPTS_PSC_LUT_SIZE";
    os = opt->longhand;
    break;
  case 2:
    ot = "environment";
    ls = "STARK_CLI_OPTS_ENV_LUT_SIZE";
    os = opt->env;
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

  enum stark_hash_table_err errc;

  if (hash_table_insert(lut, &errc, (void *)os, len, opt, 0)) {
    return true;
  }

  switch (errc) {
  case STARK_HASH_TABLE_ERR_NOT_POWER_OF_TWO:
    error(NULL, 0, "stark_cli_opts_init", "%s must be a power of two", ls);

    break;
  case STARK_HASH_TABLE_ERR_DUPLICATE:
    error(NULL, 0, "lut_insert", "duplicate %s option '%s'", ot, os);

    break;
  case STARK_HASH_TABLE_ERR_FULL:
    error(NULL, 0, "lut_insert",
          "%s option lookup table is full; define "
          "%s before "
          "inclusion with a greater limit or remove options",
          ot, ls);

    break;
  default:
    break;
  }

  return false;
}

STARK_ALWAYS_INLINE STARK_FLATTEN static inline struct stark_opt *
probe(struct stark_cli_opts *const restrict opts,
      struct stark_hash_table *const restrict lut, uint8_t const type) {
  char *restrict eq = opts->_token;

  for (; eq[0] != '=' && eq[0] != '\0'; eq++)
    ;

  if (STARK_EXPECT_FALSE(eq[0] == '\0' && type == LUT_TYPE_ENV)) {
    return NULL;
  }

  size_t tkl;
  struct stark_hash_table_bucket *bkt =
      hash_table_probe(lut, NULL, &tkl, (void *)opts->_token, eq - opts->_token,
                       HASH_TABLE_MODE_EXTRACT);

  if (bkt != NULL) {
    if (type != LUT_TYPE_PSC && eq[0] != '\0') {
      if (eq[1] == '\0') {
        eq[0] = '\0';
      } else {
        opts->_token = eq + 1;
      }
    } else {
      opts->_flags &= ~VALUE_TOKEN;
    }

    ((struct stark_opt *)bkt->val)->_fstate = LONGHAND;

    return (struct stark_opt *)bkt->val;
  }

  return NULL;
}

#endif // STARK_INTERNAL_CLI_OPTS_H
