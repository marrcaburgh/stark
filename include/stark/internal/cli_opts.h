//
// SPDX-License-Identifier: MIT
//
// stark - a C99+ utility library - stark_cli_opts - a blazing-fast feature-full
// command-line parser
// Copyright (C) 2026 marrcaburgh
//

#ifndef STARK_INTERNAL_CLI_OPTS_H
#define STARK_INTERNAL_CLI_OPTS_H

//
// Uncomment these to get syntax highlighting of the majority of the code in
// this header; the default (stack) parts will still be grayed out:
//
// #define STARK_CLI_OPTS_ENABLE_ENV
// #define STARK_CLI_OPTS_ENABLE_HEAP
//

#define STARK_CLI_OPTS_ARG_MAX_FALLBACK (1024u * 1024u)
#define STARK_CLI_OPTS_STRNAME_MAX (STARK_CLI_OPTS_ARG_MAX_FALLBACK >> 12u)

#ifdef STARK_HASH_TABLE_IMPL
#error                                                                         \
    "the implementation of hash_table must be in a separate translation unit from cli_opts"
#else // STARK_HASH_TABLE_IMPL
#ifdef STARK_HASH_TABLE_ENABLE_HEAP
#undef STARK_HASH_TABLE_ENABLE_HEAP
#define DEF_SHTEH
#endif // STARK_HASH_TABLE_ENABLE_HEAP

#ifndef STARK_HASH_TABLE_DISABLE_ERROR_PRINTING
#define STARK_HASH_TABLE_DISABLE_ERROR_PRINTING
#define UNDEF_SHTDEP
#endif // STARK_HASH_TABLE_DISABLE_ERROR_PRINTING
#include "stark/internal/hash_table.h"
#ifdef UNDEF_SHTDEP
#undef STARK_HASH_TABLE_DISABLE_ERROR_PRINTING
#endif // UNDEF_SHTDEP

#ifdef DEF_SHTEH
#define STARK_HASH_TABLE_ENABLE_HEAP
#undef DEF_SHTEH
#endif // DEF_SHTEH
#endif // STARK_HASH_TABLE_IMPL

#define LUT_TYPE_LH (0)
#define LUT_TYPE_PSC (1)
#define LUT_TYPE_ENV (2)

#define FLAG_INVALID (1u << 0u)
#define FLAG_VERIFIED (1u << 1u)
#define FLAG_VAL_TOKEN (1u << 2u)
#define FLAG_NO_VAL (1u << 3u)
#define FLAG_LONG_OPT (1u << 4u)
#define FLAG_POS_OPT (1u << 5u)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { NONE, LONGHAND, SHORTHAND, POSITIONAL, ENVIRONMENT };

STARK_COLD static void
cli_opts_error(struct stark_cli_opts *const restrict opts,
               enum stark_cli_opts_err const errc, char const *const fname_octx,
               char const *const err_ofmt, ...) {
  if (opts != NULL) {
    size_t msg_len = strlen(err_ofmt);

    if (msg_len > STARK_CLI_OPTS_STRNAME_MAX) {
      msg_len = STARK_CLI_OPTS_STRNAME_MAX;
    }

    char buf[STARK_CLI_OPTS_STRNAME_MAX + 47 + 29 + 1];

    switch (errc) {
    case STARK_CLI_OPTS_ERR_UNKNOWN_OPT:
      sprintf(buf, "unknown option: '%s%.*s'",
              opts->_flags & FLAG_POS_OPT    ? ""
              : opts->_flags & FLAG_LONG_OPT ? "--"
                                             : "-",
              (int)msg_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_BOOL_VAL:
      sprintf(buf, "unexpected value: '%.*s'", (int)msg_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_NO_VAL:
      sprintf(buf, "no value for option (expected: %s): '%s%.*s'", fname_octx,
              opts->_flags & FLAG_POS_OPT    ? ""
              : opts->_flags & FLAG_LONG_OPT ? "--"
                                             : "-",
              (int)msg_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_OOB:
      sprintf(buf, "out of bounds: '%.*s'", (int)msg_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_NAN:
      sprintf(buf, "not a number: '%.*s'", (int)msg_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_OOR:
      sprintf(buf, "out of range (expected: %s): '%.*s'", fname_octx,
              (int)msg_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_MISSING_REQUIRED_OPT:
      sprintf(buf, "missing required option: '%.*s'", (int)msg_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_CONFLICTING_OPT:
      sprintf(buf, "conflicting option %.*s", (int)msg_len, err_ofmt);

      break;
    }

    if (opts->err_callback != NULL) {
      opts->err_callback(buf);

      return;
    }

    fprintf(stderr, "%s", buf);
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
cli_opts_run_subcommand(struct stark_cli_opts *const restrict opts,
                        struct stark_cli_opt *const restrict opt) {
  int const argc = opts->_argc;

  return stark_cli_opts_parse((struct stark_cli_opts *)opt->ctx,
                              (opts->_argc = 0, argc), opts->_argv);
}

STARK_COLD STARK_ALWAYS_INLINE static inline void
cli_opts_help(STARK_UNUSED struct stark_cli_opts *const restrict opts) {
  // TODO: implement help generator with declarative configuration for
  // formatting
}

STARK_ALWAYS_INLINE static inline bool
cli_opts_assign_opt(struct stark_cli_opts *const restrict opts,
                    struct stark_cli_opt *const restrict opt) {
  if (opt->callback.callback != NULL && opt->cb_tag == 0) {
    opt->callback.callback(opt->ctx);

    return true;
  } else if (opt->assign == NULL) {
    if (opt->type == STARK_CLI_OPT_TYPE_HELP) {
      cli_opts_help(opts);

      return true;
    } else if (opt->type == STARK_CLI_OPT_TYPE_SUBCOMMAND) {
      return cli_opts_run_subcommand(opts, opt);
    }
  }

  void *vp = NULL;
  char *str = NULL, *delim = NULL, *rcp;
  char const *ot;

cli_opts_assign_opt_carr:
  if (STARK_EXPECT_FALSE(opt->mods & STARK_CLI_OPT_MOD_ARRAY &&
                         !(opt->arr_count < opt->arr_len))) {
    cli_opts_error(opts, STARK_CLI_OPTS_ERR_OOB, NULL, opts->_token);

    return false;
  }

  if (opt->type == STARK_CLI_OPT_TYPE_BOOL) {

#if defined(STARK_CLI_OPTS_ENABLE_HEAP) && defined(STARK_CLI_OPTS_ENABLE_ENV)
    if (opt->_fstate == ENVIRONMENT) {
      if (!(opts->_flags & FLAG_NO_VAL)) {
        cli_opts_error(opts, STARK_CLI_OPTS_ERR_BOOL_VAL, NULL, opts->_token);

        return false;
      }

      opts->_flags &= ~FLAG_NO_VAL;
    } else
#endif // STARK_CLI_OPTS_ENABLE_HEAP/STARK_CLI_OPTS_ENABLE_ENV

      if ((opts->_flags & FLAG_LONG_OPT) && opts->_flags & FLAG_VAL_TOKEN) {
        cli_opts_error(opts, STARK_CLI_OPTS_ERR_BOOL_VAL, NULL, opts->_token);

        return false;
      }

    ((bool *)opt->dest)[opt->arr_count] = true;

    goto cli_opts_assign_opt_skip_val;
  } else if (opt->mods & STARK_CLI_OPT_MOD_POSITIONAL) {
    goto cli_opts_assign_opt_skip_vfind_os;
  } else if (opts->_flags & FLAG_VAL_TOKEN) {
    if (opts->_flags & FLAG_NO_VAL) {
      opts->_flags &= ~FLAG_NO_VAL;

      goto cli_opts_assign_opt_vfind_failed;
    }

    opts->_flags &= ~FLAG_VAL_TOKEN;

    goto cli_opts_assign_opt_skip_vfind_os;
  } else if (opts->_argc > 1) {
  cli_opts_assign_opt_retry_narg:
    opts->_argc--;
    opts->_argv++;

    if (STARK_EXPECT_FALSE(opts->_argc == 0)) {
      goto cli_opts_assign_opt_vfind_failed;
    } else if (STARK_EXPECT_FALSE(opts->_argv == NULL ||
                                  *(opts->_argv)[0] == '\0')) {
      goto cli_opts_assign_opt_retry_narg;
    } else {
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
      strcpy(opts->_token, *opts->_argv);
#else // STARK_CLI_OPTS_ENABLE_HEAP
      opts->_token = *opts->_argv;
#endif // STARK_CLI_OPTS_ENABLE_HEAP

      goto cli_opts_assign_opt_skip_vfind_os;
    }
  }

cli_opts_assign_opt_vfind_failed:

  switch (opt->type) {
  case STARK_CLI_OPT_TYPE_INT64:
  case STARK_CLI_OPT_TYPE_INT32:
  case STARK_CLI_OPT_TYPE_INT16:
  case STARK_CLI_OPT_TYPE_INT8:
    ot = "a non-decimal number";

    break;
  case STARK_CLI_OPT_TYPE_UINT64:
  case STARK_CLI_OPT_TYPE_UINT32:
  case STARK_CLI_OPT_TYPE_UINT16:
  case STARK_CLI_OPT_TYPE_UINT8:
    ot = "a positive non-decimal number";

    break;
  case STARK_CLI_OPT_TYPE_FLOAT64:
  case STARK_CLI_OPT_TYPE_FLOAT32:
    ot = "a decimal number";

    break;
  case STARK_CLI_OPT_TYPE_STR:
    ot = "text";

    break;
  default:
    ot = "";

    break;
  }

  cli_opts_error(opts, STARK_CLI_OPTS_ERR_NO_VAL, ot, opts->_token);

  return false;

cli_opts_assign_opt_skip_vfind_os:
  str = opts->_token;

  if (opt->assign != NULL) {
    if (STARK_EXPECT_FALSE(!opt->assign(str, opt->dest, opt->arr_count, &vp))) {
      return false;
    }

    goto cli_opts_assign_opt_skip_bltn_os;
  } else if (opt->type == STARK_CLI_OPT_TYPE_STR) {
    if (opt->mods & STARK_CLI_OPT_MOD_ARRAY) {
      char *tp = str;

    cli_opts_assign_opt_fnext_delim:
      if ((delim = strchr(tp, ',')) != NULL && *delim != '\0') {
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

          goto cli_opts_assign_opt_fnext_delim;
        }
      }
    }

    ((char const **)opt->dest)[opt->arr_count] = str;
    vp = &((char const **)opt->dest)[opt->arr_count];

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
    opts->_token = strchr(str, '\0') + 1;
#endif // STARK_CLI_OPTS_ENABLE_HEAP

    goto cli_opts_assign_opt_skip_bltn_os;
  }

  char *endptr = NULL, rs[48];
  union {
    int64_t i;
    uint64_t ui;
    float f;
    double d;
  } val;
  errno = 0;

  if ((opt->type >= STARK_CLI_OPT_TYPE_INT64) &&
      (opt->type <= STARK_CLI_OPT_TYPE_INT8)) {
    val.i = strtoll(str, &endptr, 0);
  } else if ((opt->type >= STARK_CLI_OPT_TYPE_UINT64) &&
             (opt->type <= STARK_CLI_OPT_TYPE_UINT8)) {
    val.ui = strtoull(str, &endptr, 0);
  } else if (opt->type == STARK_CLI_OPT_TYPE_FLOAT64) {
    val.d = strtod(str, &endptr);
  } else if (opt->type == STARK_CLI_OPT_TYPE_FLOAT32) {
    val.f = strtof(str, &endptr);
  }

  if ((opt->mods & STARK_CLI_OPT_MOD_ARRAY) && *endptr == ',') {
    opts->_token = (delim = endptr) + 1;
    opts->_flags |= FLAG_VAL_TOKEN;
  } else if (STARK_EXPECT_FALSE(endptr == str) || *endptr != '\0') {
    cli_opts_error(opts, STARK_CLI_OPTS_ERR_NAN, NULL, str);

    return false;
  }

  switch (opt->type) {
  case STARK_CLI_OPT_TYPE_INT64:
    if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      sprintf(rs, "'%" PRId64 "' to '%" PRId64 "'", INT64_MIN, INT64_MAX);

      break;
    }

    ((int64_t *)opt->dest)[opt->arr_count] = val.i;
    vp = &((int64_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_INT32:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.i > INT32_MAX ||
                           val.i < INT32_MIN)) {
      sprintf(rs, "'%" PRId32 "' to '%" PRId32 "'", INT32_MIN, INT32_MAX);

      break;
    }

    ((int32_t *)opt->dest)[opt->arr_count] = (int32_t)val.i;
    vp = &((int32_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_INT16:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.i > INT16_MAX ||
                           val.i < INT16_MIN)) {
      sprintf(rs, "'%" PRId16 "' to '%" PRId16 "'", INT16_MIN, INT16_MAX);

      break;
    }

    ((int16_t *)opt->dest)[opt->arr_count] = (int16_t)val.i;
    vp = &((int16_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_INT8:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.i > INT8_MAX ||
                           val.i < INT8_MIN)) {
      sprintf(rs, "'%" PRId8 "' to '%" PRId8 "'", INT8_MIN, INT8_MAX);

      break;
    }

    ((int8_t *)opt->dest)[opt->arr_count] = (int8_t)val.i;
    vp = &((int8_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_UINT64:
    for (rcp = str; *rcp == ' ' || *rcp == '\t'; rcp++)
      ;

    if (STARK_EXPECT_FALSE(errno == ERANGE || rcp[0] == '-')) {
      sprintf(rs, "'%" PRIu64 "' to '%" PRIu64 "'", UINT64_C(0), UINT64_MAX);

      break;
    }

    ((uint64_t *)opt->dest)[opt->arr_count] = val.ui;
    vp = &((uint64_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_UINT32:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.ui > UINT32_MAX)) {
      sprintf(rs, "'%" PRIu32 "' to '%" PRIu32 "'", UINT32_C(0), UINT32_MAX);

      break;
    }

    ((uint32_t *)opt->dest)[opt->arr_count] = (uint32_t)val.ui;
    vp = &((uint32_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_UINT16:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.ui > UINT16_MAX)) {
      sprintf(rs, "'%" PRIu16 "' to '%" PRIu16 "'", UINT16_C(0), UINT16_MAX);

      break;
    }

    ((uint16_t *)opt->dest)[opt->arr_count] = (uint16_t)val.ui;
    vp = &((uint16_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_UINT8:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.ui > UINT8_MAX)) {
      sprintf(rs, "'%" PRIu8 "' to '%" PRIu8 "'", UINT8_C(0), UINT8_MAX);

      break;
    }

    ((uint8_t *)opt->dest)[opt->arr_count] = (uint8_t)val.ui;
    vp = &((uint8_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_FLOAT64:
    if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      sprintf(rs, "'%g' to '%g'", -DBL_MAX, DBL_MAX);

      break;
    }

    ((double *)opt->dest)[opt->arr_count] = val.d;
    vp = &((double *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_FLOAT32:
    if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      sprintf(rs, "'%g' to '%g'", -FLT_MAX, FLT_MAX);

      break;
    }

    ((float *)opt->dest)[opt->arr_count] = val.f;
    vp = &((float *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  }

  cli_opts_error(opts, STARK_CLI_OPTS_ERR_OOR, rs, str);

  return false;

cli_opts_assign_opt_skip_bltn_os:
  if (vp != NULL && opt->callback.validate != NULL && opt->cb_tag == 1 &&
      STARK_EXPECT_FALSE(!opt->callback.validate(vp, opt->ctx))) {
    return false;
  }

cli_opts_assign_opt_skip_val:
  if (opt->mods & STARK_CLI_OPT_MOD_ARRAY) {
    opt->arr_count++;

    if (delim != NULL) {
      if (opt->type == STARK_CLI_OPT_TYPE_STR) {
        *delim = '\0';
      }

      opts->_token = delim + 1;
      opts->_flags |= FLAG_VAL_TOKEN;
      delim = NULL;

      goto cli_opts_assign_opt_carr;
    }
  }

  return true;
}

STARK_COLD STARK_ALWAYS_INLINE static inline bool
cli_opts_lut_insert(struct stark_cli_opt *const restrict opt,
                    struct stark_hash_table *lut, uint8_t const type) {
  char const *ot, *ls, *os;
  size_t len = 0;

  switch (type) {
  case LUT_TYPE_LH:
    ot = "longhand";
    ls = "STARK_CLI_OPTS_LH_LUT_SIZE";
    os = opt->longhand;
    break;
  case LUT_TYPE_PSC:
    ot = "positional subcommand";
    ls = "STARK_CLI_OPTS_PSC_LUT_SIZE";
    os = opt->longhand;
    break;
  case LUT_TYPE_ENV:
    ot = "environment";
    ls = "STARK_CLI_OPTS_ENV_LUT_SIZE";
    os = opt->env;
    break;
  }

  if ((len = strlen(os)) > STARK_CLI_OPTS_STRNAME_MAX) {
    cli_opts_error(NULL, 0, "cli_opts_lut_insert",
                   "%s option '%s' length is greater than %u", ot, os,
                   STARK_CLI_OPTS_STRNAME_MAX);

    return false;
  } else if (strchr(os, '=') != NULL) {
    cli_opts_error(NULL, 0, "cli_opts_lut_insert", "%s option '%s' has an '='",
                   ot, os);

    return false;
  }

  enum stark_hash_table_err errc;

  if (hash_table_insert(lut, &errc, (void *)os, len, opt, 0)) {
    return true;
  }

  switch (errc) {
  case STARK_HASH_TABLE_ERR_NOT_POWER_OF_TWO:
    cli_opts_error(NULL, 0, "cli_opts_lut_insert", "%s must be a power of two",
                   ls);

    break;
  case STARK_HASH_TABLE_ERR_DUPLICATE:
    cli_opts_error(NULL, 0, "cli_opts_lut_insert", "duplicate %s option '%s'",
                   ot, os);

    break;
  case STARK_HASH_TABLE_ERR_FULL:
    cli_opts_error(NULL, 0, "cli_opts_lut_insert",
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

STARK_ALWAYS_INLINE STARK_FLATTEN static inline struct stark_cli_opt *
cli_opts_probe(struct stark_cli_opts *const restrict opts,
               struct stark_hash_table *const restrict lut,
               uint8_t const type) {
  char *restrict eq = opts->_token;

  for (; eq[0] != '=' && eq[0] != '\0'; eq++)
    ;

#if defined(STARK_CLI_OPTS_ENABLE_HEAP) && defined(STARK_CLI_OPTS_ENABLE_ENV)
  if (STARK_EXPECT_FALSE(eq[0] == '\0' && type == LUT_TYPE_ENV)) {
    return NULL;
  }
#endif // STARK_CLI_OPTS_ENABLE_HEAP/STARK_CLI_OPTS_ENABLE_ENV

  size_t tkl;
  struct stark_hash_table_bucket *bkt =
      hash_table_probe(lut, NULL, &tkl, (void *)opts->_token, eq - opts->_token,
                       HASH_TABLE_MODE_EXTRACT);

  if (bkt == NULL) {
    eq[0] = '\0';

    return NULL;
  }

  if (type != LUT_TYPE_PSC && eq[0] != '\0') {
    if (eq[1] == '\0') {
      eq[0] = '\0';

      opts->_flags |= FLAG_NO_VAL;
    } else {
      opts->_token = eq + 1;
    }
  } else {
    opts->_flags &= ~FLAG_VAL_TOKEN;
  }

#if defined(STARK_CLI_OPTS_ENABLE_HEAP) && defined(STARK_CLI_OPTS_ENABLE_ENV)
  if (type == LUT_TYPE_ENV) {
    ((struct stark_cli_opt *)bkt->val)->_fstate = ENVIRONMENT;
  } else {
#endif // STARK_CLI_OPTS_ENABLE_HEAP/STARK_CLI_OPTS_ENABLE_ENV

    ((struct stark_cli_opt *)bkt->val)->_fstate = LONGHAND;

#if defined(STARK_CLI_OPTS_ENABLE_HEAP) && defined(STARK_CLI_OPTS_ENABLE_ENV)
  }
#endif // STARK_CLI_OPTS_ENABLE_HEAP/STARK_CLI_OPTS_ENABLE_ENV

  return (struct stark_cli_opt *)bkt->val;
}

static inline bool cli_opts_init(struct stark_cli_opts *const restrict opts) {
  bool valid = true, vp = false;

  if (opts->_flags & FLAG_VERIFIED) {
    return true;
  } else if (opts->_flags & FLAG_INVALID) {
    return false;
  } else if (opts->optv == NULL) {
    cli_opts_error(NULL, 0, "cli_opts_init", "optv cannot be NULL");

    valid = false;

    goto cli_opts_init_loope;
  } else if (opts->optc == 0u) {
    cli_opts_error(NULL, 0, "cli_opts_init", "optc must be greater than 0");

    valid = false;

    goto cli_opts_init_loope;
  }

  memcpy(&opts->_lh_lut,
         &(struct stark_hash_table){.alg = STARK_HASH_TABLE_ALG_DJB2,
                                    .tbl_size = STARK_CLI_OPTS_LH_LUT_SIZE,
                                    .elem_size = sizeof(*opts->optv),
                                    .bkts = opts->_lh_lut_bkts},
         sizeof(opts->_lh_lut));
  memcpy(&opts->_psc_lut,
         &(struct stark_hash_table){.alg = STARK_HASH_TABLE_ALG_DJB2,
                                    .tbl_size = STARK_CLI_OPTS_PSC_LUT_SIZE,
                                    .elem_size = sizeof(*opts->optv),
                                    .bkts = opts->_psc_lut_bkts},
         sizeof(opts->_psc_lut));
  memcpy(&opts->_env_lut,
         &(struct stark_hash_table){.alg = STARK_HASH_TABLE_ALG_DJB2,
                                    .tbl_size = STARK_CLI_OPTS_ENV_LUT_SIZE,
                                    .elem_size = sizeof(*opts->optv),
                                    .bkts = opts->_env_lut_bkts},
         sizeof(opts->_env_lut));

  uint32_t li = 0;
cli_opts_init_loop:
  if (li == opts->optc) {
    goto cli_opts_init_loope;
  }

  struct stark_cli_opt *const restrict opt = &opts->optv[li];

  if (opt->mods & STARK_CLI_OPT_MOD_ARRAY && opt->arr_len <= 1) {
    cli_opts_error(NULL, 0, "cli_opts_init",
                   "array options must have an arr_len that is greater than 1");

    valid = false;
  }

  if (opt->assign != NULL) {
    if (opt->dest == NULL) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "option missing destination pointer");

      valid = false;
    }

    if (opt->type != 0) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "assigners cannot be combined with a type");

      valid = false;
    }

    if (opt->callback.callback != NULL && opt->cb_tag == 0) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "assigners cannot be combined with callbacks");

      valid = false;
    }

    goto cli_opts_init_skip_bltn;
  } else if (opt->callback.callback != NULL && opt->cb_tag == 0) {
    goto cli_opts_init_skip_bltn;
  }

  switch (opt->type) {
  case STARK_CLI_OPT_TYPE_STR:
  case STARK_CLI_OPT_TYPE_INT64:
  case STARK_CLI_OPT_TYPE_INT32:
  case STARK_CLI_OPT_TYPE_INT16:
  case STARK_CLI_OPT_TYPE_INT8:
  case STARK_CLI_OPT_TYPE_UINT64:
  case STARK_CLI_OPT_TYPE_UINT32:
  case STARK_CLI_OPT_TYPE_UINT16:
  case STARK_CLI_OPT_TYPE_UINT8:
  case STARK_CLI_OPT_TYPE_FLOAT64:
  case STARK_CLI_OPT_TYPE_FLOAT32:
  case STARK_CLI_OPT_TYPE_BOOL:
    if (opt->dest == NULL) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "option missing destination pointer");

      valid = false;
    }

    break;
  case STARK_CLI_OPT_TYPE_SUBCOMMAND:
    if (opt->ctx == NULL) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "subcommand option missing context");

      valid = false;
    }

    break;
  case STARK_CLI_OPT_TYPE_HELP:
    break;
  default:
    cli_opts_error(NULL, 0, "cli_opts_init", "unknown type: %" PRIu8,
                   opt->type);

    valid = false;

    break;
  }

cli_opts_init_skip_bltn:
  if (opt->mods & STARK_CLI_OPT_MOD_POSITIONAL) {
    if (opt->type == STARK_CLI_OPT_TYPE_SUBCOMMAND) {
      if (opt->longhand == NULL) {
        cli_opts_error(NULL, 0, "cli_opts_init",
                       "positional subcommand options must have a longhand");

        valid = false;
      } else {
        valid &= cli_opts_lut_insert(opt, &opts->_psc_lut, LUT_TYPE_PSC);
      }

      goto cli_opts_init_skip_rpos;
    } else if (opt->type == STARK_CLI_OPT_TYPE_BOOL) {
      cli_opts_error(
          NULL, 0, "cli_opts_init",
          "positional option modifier cannot be combined with boolean "
          "type");

      valid = false;
    }

    if (opts->_posc == STARK_CLI_OPTS_POS_LUT_SIZE) {
      cli_opts_error(
          NULL, 0, "cli_opts_init",
          "positional option count exceeds limit; define "
          "STARK_CLI_OPTS_POS_LUT_SIZE before inclusion with a greater limit "
          "or "
          "remove "
          "options");

      valid = false;
    } else {
      if (vp) {
        cli_opts_error(
            NULL, 0, "cli_opts_init",
            "variadic positional options must be the last positional");

        valid = false;
      }

      vp = (opt->mods & STARK_CLI_OPT_MOD_ARRAY);
      opts->_pos_lut[opts->_posc++] = opt;
    }

  cli_opts_init_skip_rpos:
    goto cli_opts_init_skip_onc;
  } else if (opt->shorthand == '\0' && opt->longhand == NULL) {
    cli_opts_error(NULL, 0, "cli_opts_init",
                   "non-positional options must have either a shorthand "
                   "or longhand");

    valid = false;
  }

  if (opt->shorthand == '\0') {
    goto cli_opts_init_skip_sh;
  }

  if (opts->_sh_lut[opt->shorthand] == NULL) {
    opts->_sh_lut[opt->shorthand] = opt;
  } else {
    cli_opts_error(NULL, 0, "cli_opts_init", "duplicate shorthand option: '%c'",
                   opt->shorthand);

    valid = false;
  }

cli_opts_init_skip_sh:
  if (opt->longhand != NULL) {
    valid &= cli_opts_lut_insert(opt, &opts->_lh_lut, LUT_TYPE_LH);
  }

#ifdef STARK_CLI_OPTS_ENABLE_ENV
  if (opt->env != NULL) {
    valid &= cli_opts_lut_insert(opt, &opts->_env_lut, LUT_TYPE_ENV);

    if (opt->mods & STARK_CLI_OPT_MOD_REQUIRED &&
        (opt->longhand != NULL || opt->shorthand != '\0')) {
      cli_opts_error(
          NULL, 0, "cli_opts_init",
          "required options cannot have both env and longhand/shorthand");

      valid = false;
    }
  }
#endif // STARK_CLI_OPTS_ENABLE_ENV

cli_opts_init_skip_onc:
  li++;

  goto cli_opts_init_loop;

cli_opts_init_loope:
  opts->_flags |= (valid ? FLAG_VERIFIED : FLAG_INVALID);

  return valid;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STARK_INTERNAL_CLI_OPTS_H

#ifdef INTERNAL_CLI_OPTS_UNDEF
#define INTERNAL_HASH_TABLE_UNDEF
#include "stark/internal/hash_table.h"
#undef INTERNAL_HASH_TABLE_UNDEF
#undef LUT_TYPE_LH
#undef LUT_TYPE_PSC
#undef LUT_TYPE_ENV
#undef FLAG_INVALID
#undef FLAG_VERIFIED
#undef FLAG_VAL_TOKEN
#undef FLAG_LONG_OPT
#undef FLAG_POS_OPT
#undef INTERNAL_CLI_OPTS_UNDEF
#endif // INTERNAL_CLI_OPTS_UNDEF
