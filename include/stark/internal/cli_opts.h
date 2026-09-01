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

#define STARK_CLI_OPTS_ARG_MAX_FALLBACK (4096u)
#define STARK_CLI_OPTS_STRNAME_MAX (256u)

#define LUT_TYPE_LH (0)
#define LUT_TYPE_POS_SBC (1)
#define LUT_TYPE_ENV (2)

#define FLAG_INVALID (1u << 0u)
#define FLAG_VERIFIED (1u << 1u)
#define FLAG_VAL_TOKEN (1u << 2u)
#define FLAG_NO_VAL (1u << 3u)
#define FLAG_LONG_OPT (1u << 4u)
#define FLAG_POS_OPT (1u << 5u)

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

#define GET_GROUP_DEP(deps, g)                                                 \
  (((deps)[(g) >> 6u] >> ((g) & 63u)) & UINT64_C(1))

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define FOUND_NONE 0u
#define FOUND_LONGHAND 1u
#define FOUND_SHORTHAND 2u
#define FOUND_POSITIONAL 3u
#define FOUND_ENVIRONMENT 4u

STARK_COLD static void
cli_opts_error(struct stark_cli_opts *const restrict opts,
               enum stark_cli_opts_err const errc, char const *const fname_octx,
               char const *const err_ofmt, ...) {

  if (opts != NULL) {
    size_t ctx_len, fmt_len = strlen(err_ofmt);

    if (fmt_len > STARK_CLI_OPTS_STRNAME_MAX) {
      fmt_len = STARK_CLI_OPTS_STRNAME_MAX;
    }

    if (fname_octx != NULL &&
        (ctx_len = strlen(fname_octx)) > STARK_CLI_OPTS_STRNAME_MAX) {
      ctx_len = STARK_CLI_OPTS_STRNAME_MAX;
    }

    char buf[STARK_CLI_OPTS_STRNAME_MAX * 2 + 53];

    switch (errc) {
    case STARK_CLI_OPTS_ERR_UNKNOWN_OPT:
      sprintf(buf, "unknown option: '%s%.*s'",
              opts->_flags & FLAG_POS_OPT    ? ""
              : opts->_flags & FLAG_LONG_OPT ? "--"
                                             : "-",
              (int)fmt_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_UNEXPECTED_VAL:
      sprintf(buf, "unexpected value: '%.*s'", (int)fmt_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_NO_VAL:
      sprintf(buf, "no value for option (expected: %s): '%s%.*s'", fname_octx,
              opts->_flags & FLAG_POS_OPT    ? ""
              : opts->_flags & FLAG_LONG_OPT ? "--"
                                             : "-",
              (int)fmt_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_OOB:
      sprintf(buf, "out of bounds: '%.*s'", (int)fmt_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_NAN:
      sprintf(buf, "not a number: '%.*s'", (int)fmt_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_OOR:
      sprintf(buf, "out of range (expected: %s): '%.*s'", fname_octx,
              (int)fmt_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_MISSING_REQUIRED_OPT:
      sprintf(buf, "missing required option: '%.*s'", (int)fmt_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_CONFLICTING_OPT:
      sprintf(buf, "conflicting option %.*s", (int)fmt_len, err_ofmt);

      break;
    case STARK_CLI_OPTS_ERR_MISSING_OPT_DEPENDENCY:
      sprintf(buf, "option '%.*s' depends on option '%.*s'", (int)fmt_len,
              err_ofmt, (int)ctx_len, fname_octx);

      break;
    default:
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

STARK_NOINLINE static enum stark_cli_opts_err
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

STARK_ALWAYS_INLINE static inline enum stark_cli_opts_err
cli_opts_assign_opt(struct stark_cli_opts *const restrict opts,
                    struct stark_cli_opt *const restrict opt) {
  if (opt->callback.callback != NULL && opt->cb_tag == 0) {
    opt->callback.callback(opt->ctx);

    return STARK_CLI_OPTS_ERR_SUCCESS;
  } else if (opt->assign == NULL) {
    if (opt->type == STARK_CLI_OPT_TYPE_HELP) {
      cli_opts_help(opts);

      return STARK_CLI_OPTS_ERR_EXIT;
    } else if (opt->type == STARK_CLI_OPT_TYPE_VERSION) {
      printf("%s %s\n", opts->name, opts->version);

      return STARK_CLI_OPTS_ERR_EXIT;
    } else if (opt->type == STARK_CLI_OPT_TYPE_SUBCOMMAND) {
      return cli_opts_run_subcommand(opts, opt);
    }
  }

  void *vp = NULL;
  char *str = NULL, *delim = NULL;
  char const *opt_type;

cli_opts_assign_opt_carr:
  if (STARK_EXPECT_FALSE(opt->mods & STARK_CLI_OPT_MOD_ARRAY &&
                         opt->arr_count == opt->arr_len)) {
    cli_opts_error(opts, STARK_CLI_OPTS_ERR_OOB, NULL, opts->_token);

    return STARK_CLI_OPTS_ERR_FAILURE;
  }

  if (opt->type == STARK_CLI_OPT_TYPE_BOOL) {
#if defined(STARK_CLI_OPTS_ENABLE_HEAP) && defined(STARK_CLI_OPTS_ENABLE_ENV)
    if (opt->_found_state == FOUND_ENVIRONMENT) {
      if (!(opts->_flags & FLAG_NO_VAL)) {
        cli_opts_error(opts, STARK_CLI_OPTS_ERR_UNEXPECTED_VAL, NULL,
                       opts->_token);

        return STARK_CLI_OPTS_ERR_FAILURE;
      }

      opts->_flags &= ~FLAG_NO_VAL;
    } else
#endif // STARK_CLI_OPTS_ENABLE_HEAP/STARK_CLI_OPTS_ENABLE_ENV

      if ((opts->_flags & FLAG_LONG_OPT) && opts->_flags & FLAG_VAL_TOKEN) {
        cli_opts_error(opts, STARK_CLI_OPTS_ERR_UNEXPECTED_VAL, NULL,
                       opts->_token);

        return STARK_CLI_OPTS_ERR_FAILURE;
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
    } else if (STARK_EXPECT_FALSE(*opts->_argv == NULL ||
                                  *(opts->_argv)[0] == '\0')) {
      goto cli_opts_assign_opt_retry_narg;
    } else {
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
      strcpy(opts->_token, *opts->_argv);
#else  // STARK_CLI_OPTS_ENABLE_HEAP
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
    opt_type = "a non-decimal number";

    break;
  case STARK_CLI_OPT_TYPE_UINT64:
  case STARK_CLI_OPT_TYPE_UINT32:
  case STARK_CLI_OPT_TYPE_UINT16:
  case STARK_CLI_OPT_TYPE_UINT8:
    opt_type = "a positive non-decimal number";

    break;
  case STARK_CLI_OPT_TYPE_FLOAT64:
  case STARK_CLI_OPT_TYPE_FLOAT32:
    opt_type = "a decimal number";

    break;
  case STARK_CLI_OPT_TYPE_STR:
    opt_type = "text";

    break;
  default:
    opt_type = "";

    break;
  }

  cli_opts_error(opts, STARK_CLI_OPTS_ERR_NO_VAL, opt_type, opts->_token);

  return STARK_CLI_OPTS_ERR_FAILURE;

cli_opts_assign_opt_skip_vfind_os:

  str = opts->_token;

  if (opt->assign != NULL) {
    if (STARK_EXPECT_FALSE(!opt->assign(str, opt->dest, opt->arr_count, &vp))) {
      return STARK_CLI_OPTS_ERR_FAILURE;
    }

    goto cli_opts_assign_opt_skip_bltn_os;
  } else if (opt->type == STARK_CLI_OPT_TYPE_STR) {
    if (opt->mods & STARK_CLI_OPT_MOD_ARRAY) {
      char *tp = str;

    cli_opts_assign_opt_fnext_delim:
      if ((delim = strchr(tp, ':')) != NULL) {
        bool esc = false;

        for (tp = delim; tp > str && *--tp == '\\'; esc = !esc)
          ;

        if (esc) {
          str = memmove(&str[delim - tp - 1], str, tp - str + 1);
          tp = &delim[1];

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

  char *endptr = NULL, range_str[48];
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

  if ((opt->mods & STARK_CLI_OPT_MOD_ARRAY) && *endptr == ':') {
    opts->_token = (delim = endptr) + 1;
    opts->_flags |= FLAG_VAL_TOKEN;
  } else if (STARK_EXPECT_FALSE(*endptr != '\0')) {
    cli_opts_error(opts, STARK_CLI_OPTS_ERR_NAN, NULL, str);

    return STARK_CLI_OPTS_ERR_FAILURE;
  }

  switch (opt->type) {
  case STARK_CLI_OPT_TYPE_INT64:
    if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      sprintf(range_str, "'%" PRId64 "' to '%" PRId64 "'", INT64_MIN,
              INT64_MAX);

      break;
    }

    ((int64_t *)opt->dest)[opt->arr_count] = val.i;
    vp = &((int64_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_INT32:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.i > INT32_MAX ||
                           val.i < INT32_MIN)) {
      sprintf(range_str, "'%" PRId32 "' to '%" PRId32 "'", INT32_MIN,
              INT32_MAX);

      break;
    }

    ((int32_t *)opt->dest)[opt->arr_count] = (int32_t)val.i;
    vp = &((int32_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_INT16:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.i > INT16_MAX ||
                           val.i < INT16_MIN)) {
      sprintf(range_str, "'%" PRId16 "' to '%" PRId16 "'", INT16_MIN,
              INT16_MAX);

      break;
    }

    ((int16_t *)opt->dest)[opt->arr_count] = (int16_t)val.i;
    vp = &((int16_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_INT8:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.i > INT8_MAX ||
                           val.i < INT8_MIN)) {
      sprintf(range_str, "'%" PRId8 "' to '%" PRId8 "'", INT8_MIN, INT8_MAX);

      break;
    }

    ((int8_t *)opt->dest)[opt->arr_count] = (int8_t)val.i;
    vp = &((int8_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_UINT64:
    if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      sprintf(range_str, "'%" PRIu64 "' to '%" PRIu64 "'", UINT64_C(0),
              UINT64_MAX);

      break;
    }

    ((uint64_t *)opt->dest)[opt->arr_count] = val.ui;
    vp = &((uint64_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_UINT32:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.ui > UINT32_MAX)) {
      sprintf(range_str, "'%" PRIu32 "' to '%" PRIu32 "'", UINT32_C(0),
              UINT32_MAX);

      break;
    }

    ((uint32_t *)opt->dest)[opt->arr_count] = (uint32_t)val.ui;
    vp = &((uint32_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_UINT16:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.ui > UINT16_MAX)) {
      sprintf(range_str, "'%" PRIu16 "' to '%" PRIu16 "'", UINT16_C(0),
              UINT16_MAX);

      break;
    }

    ((uint16_t *)opt->dest)[opt->arr_count] = (uint16_t)val.ui;
    vp = &((uint16_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_UINT8:
    if (STARK_EXPECT_FALSE(errno == ERANGE || val.ui > UINT8_MAX)) {
      sprintf(range_str, "'%" PRIu8 "' to '%" PRIu8 "'", UINT8_C(0), UINT8_MAX);

      break;
    }

    ((uint8_t *)opt->dest)[opt->arr_count] = (uint8_t)val.ui;
    vp = &((uint8_t *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_FLOAT64:
    if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      sprintf(range_str, "'%g' to '%g'", -DBL_MAX, DBL_MAX);

      break;
    }

    ((double *)opt->dest)[opt->arr_count] = val.d;
    vp = &((double *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  case STARK_CLI_OPT_TYPE_FLOAT32:
    if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      sprintf(range_str, "'%g' to '%g'", -FLT_MAX, FLT_MAX);

      break;
    }

    ((float *)opt->dest)[opt->arr_count] = val.f;
    vp = &((float *)opt->dest)[opt->arr_count];

    goto cli_opts_assign_opt_skip_bltn_os;
  }

  cli_opts_error(opts, STARK_CLI_OPTS_ERR_OOR, range_str, str);

  return STARK_CLI_OPTS_ERR_FAILURE;

cli_opts_assign_opt_skip_bltn_os:
  if (vp != NULL && opt->callback.validate != NULL && opt->cb_tag == 1 &&
      STARK_EXPECT_FALSE(!opt->callback.validate(vp, opt->ctx))) {
    return STARK_CLI_OPTS_ERR_FAILURE;
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

  return STARK_CLI_OPTS_ERR_SUCCESS;
}

STARK_COLD STARK_ALWAYS_INLINE static inline bool
cli_opts_lut_insert(struct stark_cli_opts *const restrict opts,
                    struct stark_cli_opt *const restrict opt,
                    uint8_t const lut_type) {
  struct stark_hash_table *restrict lut;
  uint8_t *restrict idx_lut;

  switch (lut_type) {
  case LUT_TYPE_LH:
    lut = &opts->_lh_lut;
    idx_lut = opts->_lh_idx_lut;

    break;
  case LUT_TYPE_POS_SBC:
    lut = &opts->_pos_sbc_lut;
    idx_lut = opts->_pos_sbc_idx_lut;

    break;
  case LUT_TYPE_ENV:
    lut = &opts->_env_lut;
    idx_lut = opts->_env_idx_lut;

    break;
  }

  for (uint8_t i = 0; i <= 8; i++) {
    char const *opt_type, *opt_str;
    size_t len;

    switch (lut_type) {
    case LUT_TYPE_LH:
      opt_type = "longhand";
      opt_str = opt->longhand[i];

      break;
    case LUT_TYPE_POS_SBC:
      opt_type = "positional subcommand";
      opt_str = opt->longhand[i];

      break;
    case LUT_TYPE_ENV:
      opt_type = "environment";
      opt_str = opt->env[i];

      break;
    }

    if (opt_str == NULL) {
      return true;
    } else if (i == 8) {
      cli_opts_error(NULL, 0, "cli_opts_lut_insert",
                     "alias arrays must be followed by a NULL terminator");

      return false;
    }

    if ((len = strlen(opt_str)) > STARK_CLI_OPTS_STRNAME_MAX) {
      cli_opts_error(NULL, 0, "cli_opts_lut_insert",
                     "%s option '%s' length is greater than %u", opt_type,
                     opt_str, STARK_CLI_OPTS_STRNAME_MAX);

      return false;
    } else if (strchr(opt_str, '=') != NULL) {
      cli_opts_error(NULL, 0, "cli_opts_lut_insert",
                     "%s option '%s' has an '='", opt_type, opt_str);

      return false;
    }

    enum stark_hash_table_err errc;

    struct stark_hash_table_bucket *bkt =
        hash_table_insert(lut, &errc, (void *)opt_str, len, opt, 0);

    if (bkt != NULL) {
      idx_lut[bkt->idx] = i;

      continue;
    }

    switch (errc) {
    case STARK_HASH_TABLE_ERR_DUPLICATE:
      cli_opts_error(NULL, 0, "cli_opts_lut_insert", "duplicate %s option '%s'",
                     opt_type, opt_str);

      break;
    case STARK_HASH_TABLE_ERR_FULL:
      cli_opts_error(NULL, 0, "cli_opts_lut_insert",
                     "%s option lookup table is full; remove options",
                     opt_type);

      break;
    default:
      break;
    }

    return false;
  }

  return true;
}

STARK_ALWAYS_INLINE STARK_FLATTEN static inline struct stark_cli_opt *
cli_opts_probe(struct stark_cli_opts *const restrict opts,
               uint8_t const lut_type) {
  struct stark_hash_table *restrict lut;
  uint8_t *restrict idx_lut;
  char *restrict eq = opts->_token;

  switch (lut_type) {
  case LUT_TYPE_LH:
    lut = &opts->_lh_lut;
    idx_lut = opts->_lh_idx_lut;

    break;
  case LUT_TYPE_ENV:
    lut = &opts->_env_lut;
    idx_lut = opts->_env_idx_lut;

    break;
  case LUT_TYPE_POS_SBC:
    lut = &opts->_pos_sbc_lut;
    idx_lut = opts->_pos_sbc_idx_lut;

    break;
  }

  for (; eq[0] != '=' && eq[0] != '\0'; eq++)
    ;

#if defined(STARK_CLI_OPTS_ENABLE_HEAP) && defined(STARK_CLI_OPTS_ENABLE_ENV)
  if (STARK_EXPECT_FALSE(eq[0] == '\0' && lut_type == LUT_TYPE_ENV)) {
    return NULL;
  }
#endif // STARK_CLI_OPTS_ENABLE_HEAP/STARK_CLI_OPTS_ENABLE_ENV

  struct stark_hash_table_bucket *bkt =
      hash_table_probe(lut, NULL, NULL, NULL, (void *)opts->_token,
                       eq - opts->_token, HASH_TABLE_MODE_EXTRACT);

  if (bkt == NULL) {
    eq[0] = '\0';

    return NULL;
  }

  if (lut_type != LUT_TYPE_POS_SBC && eq[0] != '\0') {
    if (eq[1] == '\0') {
      eq[0] = '\0';

      opts->_flags |= FLAG_NO_VAL;
    } else {
      opts->_token = &eq[1];
    }
  } else {
    opts->_flags &= ~FLAG_VAL_TOKEN;
  }

  struct stark_cli_opt *opt = (struct stark_cli_opt *)bkt->val;
  opt->_found_idx = idx_lut[bkt->idx];

#if defined(STARK_CLI_OPTS_ENABLE_HEAP) && defined(STARK_CLI_OPTS_ENABLE_ENV)
  if (lut_type == LUT_TYPE_ENV) {
    ((struct stark_cli_opt *)bkt->val)->_found_state = FOUND_ENVIRONMENT;
  } else
#endif // STARK_CLI_OPTS_ENABLE_HEAP/STARK_CLI_OPTS_ENABLE_ENV
    if (lut_type == LUT_TYPE_LH) {
      opt->_found_state = FOUND_LONGHAND;
    } else {
      opt->_found_state = FOUND_POSITIONAL;
    }

  return opt;
}

static inline bool cli_opts_init(struct stark_cli_opts *const restrict opts) {
  bool valid = true, vpos = false, npos_seen = false, pos_sbc_seen = false,
       gmode = false;

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
  } else if (opts->goptv != NULL && opts->goptc == 0) {
    cli_opts_error(NULL, 0, "cli_opts_init", "goptc must be greater than 0");

    valid = false;
  }

  memcpy(&opts->_lh_lut,
         &(struct stark_hash_table){.alg = STARK_HASH_TABLE_ALG_FNV1A,
                                    .tbl_size = STARK_CLI_OPTS_LONGHAND_MAX,
                                    .elem_size = sizeof(*opts->optv),
                                    .bkts = opts->_lh_lut_bkts},
         sizeof(opts->_lh_lut));
  memcpy(&opts->_pos_sbc_lut,
         &(struct stark_hash_table){
             .alg = STARK_HASH_TABLE_ALG_FNV1A,
             .tbl_size = STARK_CLI_OPTS_POSITIONAL_SUBCOMMAND_MAX,
             .elem_size = sizeof(*opts->optv),
             .bkts = opts->_pos_sbc_lut_bkts},
         sizeof(opts->_pos_sbc_lut));
  memcpy(&opts->_env_lut,
         &(struct stark_hash_table){.alg = STARK_HASH_TABLE_ALG_FNV1A,
                                    .tbl_size = STARK_CLI_OPTS_ENVIRONMENT_MAX,
                                    .elem_size = sizeof(*opts->optv),
                                    .bkts = opts->_env_lut_bkts},
         sizeof(opts->_env_lut));

  uint32_t li = 0;
cli_opts_init_loop:
  struct stark_cli_opt *restrict opt;

  if (li == (gmode ? opts->goptc : opts->optc)) {
    goto cli_opts_init_loope;
  }

  opt = &(gmode ? opts->goptv : opts->optv)[li];

  if (opt->mods & STARK_CLI_OPT_MOD_ARRAY && opt->arr_len <= 1) {
    cli_opts_error(NULL, 0, "cli_opts_init",
                   "array options must have an arr_len that is greater than 1");

    valid = false;
  }

  if (opt->mods & STARK_CLI_OPT_MOD_GROUP) {
    if (!GET_GROUP_DEP(opts->group_deps, opt->group) &&
        (opt->mods & STARK_CLI_OPT_MOD_REQUIRED)) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "exclusive option groups cannot contain required options");

      valid = false;
    }

#ifndef STARK_CLI_OPTS_ENABLE_HEAP
    cli_opts_error(NULL, 0, "cli_opts_init",
                   "option groups require STARK_CLI_OPTS_ENABLE_HEAP");

    valid = false;
#endif // STARK_CLI_OPTS_ENABLE_HEAP
  }

  if (opt->env != NULL) {
#ifndef STARK_CLI_OPTS_ENABLE_ENV
    cli_opts_error(NULL, 0, "cli_opts_init",
                   "env options require STARK_CLI_OPTS_ENABLE_ENV");

    valid = false;
#endif // STARK_CLI_OPTS_ENABLE_ENV
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
    if (gmode) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "subcommand options cannot be global options");

      valid = false;
    }

    if (opt->env != NULL) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "subcommand options cannot have env");

      valid = false;
    }

    if (opt->ctx == NULL) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "subcommand option missing context");

      valid = false;
    } else {
      struct stark_cli_opts *sbc_opts = (struct stark_cli_opts *)opt->ctx;

      if (sbc_opts->goptv != NULL) {
        cli_opts_error(NULL, 0, "cli_opts_init",
                       "subcommand options cannot have goptv set");

        valid = false;
      }

      if (sbc_opts->goptc != 0) {
        cli_opts_error(NULL, 0, "cli_opts_init",
                       "subcommand options cannot have goptc set");

        valid = false;
      }

      if (valid && !(opt->mods & STARK_CLI_OPT_MOD_NO_GLOBALS) &&
          opts->goptv != NULL) {
        sbc_opts->goptv = opts->goptv;
        sbc_opts->goptc = opts->goptc;
      }
    }

    break;
  case STARK_CLI_OPT_TYPE_HELP:
    break;
  case STARK_CLI_OPT_TYPE_VERSION:
    if (opts->version == NULL) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "version options require the version field to be set");

      valid = false;
    }

    if (opts->name == NULL) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "version options require the name field to be set");

      valid = false;
    }

    break;
  default:
    cli_opts_error(NULL, 0, "cli_opts_init", "option has unknown type");

    valid = false;

    break;
  }

cli_opts_init_skip_bltn:
  if (gmode && (opt->mods & STARK_CLI_OPT_MOD_REQUIRED)) {
    cli_opts_error(NULL, 0, "cli_opts_init",
                   "global options cannot be required");

    valid = false;
  }

  if (opt->mods & STARK_CLI_OPT_MOD_POSITIONAL) {
    if (gmode) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "positional options cannot be global options");

      valid = false;

      goto cli_opts_init_pglobal;
    }

    if (opt->type == STARK_CLI_OPT_TYPE_SUBCOMMAND) {
      if (npos_seen) {
        cli_opts_error(
            NULL, 0, "cli_opts_init",
            "positional subcommand options must precede non-positional "
            "options");

        valid = false;
      }

      pos_sbc_seen = true;

      if (opt->longhand == NULL) {
        cli_opts_error(NULL, 0, "cli_opts_init",
                       "positional subcommand options must have a longhand");

        valid = false;
      } else {
        valid &= cli_opts_lut_insert(opts, opt, LUT_TYPE_POS_SBC);
      }

      goto cli_opts_init_skip_rpos;
    } else if (opt->type == STARK_CLI_OPT_TYPE_BOOL) {
      cli_opts_error(
          NULL, 0, "cli_opts_init",
          "positional option modifier cannot be combined with boolean "
          "type");

      valid = false;
    }

    if (pos_sbc_seen || npos_seen) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "positional options must precede all other options");

      valid = false;
    }

    if (opts->_posc == STARK_CLI_OPTS_POSITIONAL_MAX) {
      cli_opts_error(NULL, 0, "cli_opts_init",
                     "positional option count exceeds limit; remove options");

      valid = false;
    } else {
      if (vpos) {
        cli_opts_error(
            NULL, 0, "cli_opts_init",
            "variadic positional options must be the last positional");

        valid = false;
      }

      vpos = (opt->mods & STARK_CLI_OPT_MOD_ARRAY);
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

cli_opts_init_pglobal:
  if (opt->shorthand == '\0') {
    goto cli_opts_init_skip_sh;
  }

  if (opts->_sh_lut[opt->shorthand - 1] == NULL) {
    opts->_sh_lut[opt->shorthand - 1] = opt;
  } else {
    cli_opts_error(NULL, 0, "cli_opts_init", "duplicate shorthand option: '%c'",
                   opt->shorthand);

    valid = false;
  }

cli_opts_init_skip_sh:
  if (opt->longhand != NULL) {
    valid &= cli_opts_lut_insert(opts, opt, LUT_TYPE_LH);
  }

#ifdef STARK_CLI_OPTS_ENABLE_ENV
  if (opt->env != NULL) {
    valid &= cli_opts_lut_insert(opts, opt, LUT_TYPE_ENV);

    if (opt->longhand != NULL || opt->shorthand != '\0') {
      if (opt->mods & STARK_CLI_OPT_MOD_REQUIRED) {
        cli_opts_error(
            NULL, 0, "cli_opts_init",
            "required options cannot have both env and longhand/shorthand");

        valid = false;
      }
      if (opt->mods & STARK_CLI_OPT_MOD_GROUP) {
        cli_opts_error(NULL, 0, "cli_opts_init",
                       "env options cannot be in the same group as "
                       "longhand/shorthand options");
        valid = false;
      }
    }
  }
#endif // STARK_CLI_OPTS_ENABLE_ENV

cli_opts_init_skip_onc:
  if (!(opt->mods & STARK_CLI_OPT_MOD_POSITIONAL)) {
    npos_seen = true;
  }

  li++;

  goto cli_opts_init_loop;

cli_opts_init_loope:
  if (opts->goptv != NULL && !gmode) {
    gmode = true;
    li = 0;

    goto cli_opts_init_loop;
  }

  opts->_flags |= (valid ? FLAG_VERIFIED : FLAG_INVALID);

  return valid;
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // STARK_INTERNAL_CLI_OPTS_H

#if !defined(STARK_INTERNAL_CLI_OPTS_SKIP_UNDEF) &&                            \
    defined(INTERNAL_CLI_OPTS_UNDEF)
#define STARK_INTERNAL_CLI_OPTS_SKIP_UNDEF
#define INTERNAL_HASH_TABLE_UNDEF
#include "stark/internal/hash_table.h"

#undef INTERNAL_HASH_TABLE_UNDEF
#undef LUT_TYPE_LH
#undef LUT_TYPE_POS_SBC
#undef LUT_TYPE_ENV
#undef FLAG_INVALID
#undef FLAG_VERIFIED
#undef FLAG_VAL_TOKEN
#undef FLAG_LONG_OPT
#undef FLAG_POS_OPT
#undef FOUND_NONE
#undef FOUND_LONGHAND
#undef FOUND_SHORTHAND
#undef FOUND_POSITIONAL
#undef FOUND_ENVIRONMENT
#undef INTERNAL_CLI_OPTS_UNDEF
#endif // !STARK_INTERNAL_CLI_OPTS_SKIP_UNDEF && INTERNAL_CLI_OPTS_UNDEF
