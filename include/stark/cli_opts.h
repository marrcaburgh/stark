//
// SPDX-License-Identifier: MIT
//
// stark - a C99+ utility library - stark_cli_opts - a blazing-fast feature-full
// command-line parser
// Copyright (C) 2026 marrcaburgh
//

#ifndef STARK_CLI_OPTS_H
#define STARK_CLI_OPTS_H

//
// Uncomment these to get syntax highlighting of the majority of the code in
// this header; the default (stack) parts will still be grayed out:
//
// #define STARK_CLI_OPTS_ENABLE_ENV
// #define STARK_CLI_OPTS_ENABLE_HEAP
// #define STARK_CLI_OPTS_IMPL
//
//
// Define these macros before including this header or with your build system:
//
// Define STARK_CLI_OPTS_LH_LUT_SIZE, STARK_CLI_OPTS_POS_LUT_SIZE,
// STARK_CLI_OPTS_PSC_LUT_SIZE, and/or STARK_CLI_OPTS_ENV_LUT_SIZE to change the
// size of each corresponding lookup table. Each LUT except for POS (positional)
// must be a power of two. The default sizes are listed below.
//
// Define STARK_CLI_OPTS_ENABLE_ENV to enable environment variable parsing if
// you're on an operating system that defines `environ` or `_environ` as part of
// the C standard library. See below.
//
// Define STARK_CLI_OPTS_ENABLE_HEAP to enable heap allocation for a worst-case
// sized token pool that is used to tokenize argv and slice delimited string
// arrays, and the environment if environment variable parsing is enabled.
//
// Delimited string arrays can be used with heap allocation disabled, but
// argv must point to mutable strings; otherwise, this is undefined behavior.
//
#ifndef STARK_CLI_OPTS_LH_LUT_SIZE
#define STARK_CLI_OPTS_LH_LUT_SIZE (64u)
#endif // STARK_CLI_OPTS_LH_LUT_SIZE
#ifndef STARK_CLI_OPTS_POS_LUT_SIZE
#define STARK_CLI_OPTS_POS_LUT_SIZE (4u)
#endif // STARK_CLI_OPTS_POS_LUT_SIZE
#ifndef STARK_CLI_OPTS_PSC_LUT_SIZE
#define STARK_CLI_OPTS_PSC_LUT_SIZE (8u)
#endif // STARK_CLI_OPTS_PSC_LUT_SIZE
#ifndef STARK_CLI_OPTS_ENV_LUT_SIZE
#define STARK_CLI_OPTS_ENV_LUT_SIZE (16u)
#endif // STARK_CLI_OPTS_ENV_LUT_SIZE

#include "stark/core.h"
#include "stark/hash_table.h"

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum stark_cli_opts_err {
  STARK_CLI_OPTS_ERR_UNKNOWN_OPT = 1u,
  STARK_CLI_OPTS_ERR_BOOL_VAL = 2u,
  STARK_CLI_OPTS_ERR_NO_VAL = 3u,
  STARK_CLI_OPTS_ERR_OOB = 4u,
  STARK_CLI_OPTS_ERR_NAN = 5u,
  STARK_CLI_OPTS_ERR_OOR = 6u,
  STARK_CLI_OPTS_ERR_MISSING_REQUIRED_OPT = 7u,
  STARK_CLI_OPTS_ERR_CONFLICTING_OPT = 8u,
} stark_cli_opts_err_t;

enum {
  /* regular types */
  STARK_CLI_OPT_TYPE_BOOL = 1u,
  STARK_CLI_OPT_TYPE_INT64 = 2u,
  STARK_CLI_OPT_TYPE_INT32 = 3u,
  STARK_CLI_OPT_TYPE_INT16 = 4u,
  STARK_CLI_OPT_TYPE_INT8 = 5u,
  STARK_CLI_OPT_TYPE_UINT64 = 6u,
  STARK_CLI_OPT_TYPE_UINT32 = 7u,
  STARK_CLI_OPT_TYPE_UINT16 = 8u,
  STARK_CLI_OPT_TYPE_UINT8 = 9u,
  STARK_CLI_OPT_TYPE_FLOAT64 = 10u,
  STARK_CLI_OPT_TYPE_FLOAT32 = 11u,
  STARK_CLI_OPT_TYPE_STR = 12u,

  /* special types */
  STARK_CLI_OPT_TYPE_SUBCOMMAND = 13u,

  /* bltn types */
  STARK_CLI_OPT_TYPE_HELP = 14u,
};

enum {
  STARK_CLI_OPT_MOD_REQUIRED = (1u << 0u),
  STARK_CLI_OPT_MOD_HIDDEN = (1u << 1u),
  STARK_CLI_OPT_MOD_POSITIONAL = (1u << 2u),
  STARK_CLI_OPT_MOD_ARRAY = (1u << 3u),
};

typedef struct STARK_ALIGNED(64u) stark_cli_opt {
  uint8_t type : 4u;
  uint8_t mods : 4u;
  uint8_t cb_tag : 1u;
  uint8_t _fstate : 3u;
  uint8_t _pad : 4u;
  uint8_t arr_count;
  uint8_t const arr_len;
  unsigned char const shorthand;
  uint8_t group;
  uint16_t _pad1;
  char const *const restrict longhand;
  char const *const restrict env;
  void *const restrict dest;
  bool (*const assign)(char const *const restrict str,
                       void *const restrict dest, uint8_t const arr_count,
                       void *restrict *const restrict vpp);
  const union {
    void (*const callback)(const void *const restrict ctx);
    bool (*const validate)(const void *const restrict val,
                           const void *const restrict ctx);
  } callback;
  void *const restrict ctx;
  char const *const restrict usage;
} stark_cli_opt_t; // Fits into one 64-bit L1 cache line or 64 bytes of memory.

typedef struct stark_cli_opts {
  struct stark_hash_table _lh_lut, _psc_lut, _env_lut;
  struct stark_hash_table_bucket _lh_lut_bkts[STARK_CLI_OPTS_LH_LUT_SIZE],
      _psc_lut_bkts[STARK_CLI_OPTS_PSC_LUT_SIZE],
      _env_lut_bkts[STARK_CLI_OPTS_ENV_LUT_SIZE];
  struct stark_cli_opt *const optv, **_group_table[UINT8_MAX],
      *_sh_lut[UCHAR_MAX + 1u], *_pos_lut[STARK_CLI_OPTS_POS_LUT_SIZE];
  void (*const err_callback)(char const *const restrict err_msg);
  char *_token_pool, *_token, **_argv;
  char const *const restrict desc;
  int _argc;
  uint32_t _posc;
  uint16_t const optc;
  uint8_t _flags;
} stark_cli_opts_t;

bool stark_cli_opts_parse(struct stark_cli_opts *opts, int argc, char **argv);
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
void stark_cli_opts_free_token_pools(struct stark_cli_opts *opts);
void stark_cli_opts_free_group_pools(struct stark_cli_opts *opts);
#endif // STARK_CLI_OPTS_ENABLE_HEAP

#ifdef STARK_CLI_OPTS_IMPL

#define FLAG_TP_DIRTY (1u << 6u)
#define FLAG_GPS_DIRTY (1u << 7u)

#include "stark/internal/cli_opts.h"

#ifdef __linux__
#include <linux/limits.h>
#endif // __linux__
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool stark_cli_opts_parse(struct stark_cli_opts *const restrict opts,
                          int const argc, char **argv) {
  if (STARK_EXPECT_FALSE(opts == NULL)) {
    cli_opts_error(NULL, 0, "stark_cli_opts_parse", "cli_opts cannot be NULL");

    return false;
  } else if (STARK_EXPECT_FALSE(argv == NULL)) {
    cli_opts_error(NULL, 0, "stark_cli_opts_parse", "argv cannot be NULL");

    return false;
  } else if (STARK_EXPECT_FALSE(argc <= 0)) {
    cli_opts_error(NULL, 0, "stark_cli_opts_parse",
                   "argc must be greater than 0");

    return false;
  } else if (STARK_EXPECT_FALSE(!cli_opts_init(opts))) {
    return false;
  } else if (STARK_EXPECT_FALSE(!(opts->_flags & FLAG_VERIFIED))) {
    return false;
  } else if (STARK_EXPECT_FALSE((opts->_flags & FLAG_INVALID))) {
    return false;
  }

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  else if (STARK_EXPECT_FALSE(((opts->_flags & FLAG_TP_DIRTY) &&
                               opts->_token_pool != NULL &&
                               opts->_token != NULL))) {
    cli_opts_error(NULL, 0, "stark_cli_opts_parse",
                   "dirty; did you forget 'stark_cli_opts_free_token_pool()'?");

    return false;
  }
#endif // STARK_CLI_OPTS_ENABLE_HEAP

  struct stark_cli_opt *opt;

  for (uint32_t i = 0; i < opts->optc; i++) {
    opt = &opts->optv[i];
    opt->_fstate = NONE;
    opt->arr_count = 0;

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
    if (!(opts->_flags & FLAG_GPS_DIRTY) && opt->group != 0) {
      size_t count = 1;

      if (opts->_group_table[opt->group - 1] == NULL) {
        opts->_group_table[opt->group - 1] =
            malloc(sizeof(size_t) + opts->optc * sizeof(void *));

        if (opts->_group_table[opt->group - 1] == NULL) {
          cli_opts_error(NULL, 0, "stark_cli_opts_parse",
                         "allocation for group pool failed");

          return false;
        }

        opts->_group_table[opt->group - 1] =
            (struct stark_cli_opt **)(((size_t *)
                                           opts->_group_table[opt->group - 1]) +
                                      1);
      } else {
        count += *(((size_t *)opts->_group_table[opt->group - 1]) - 1);
      }

      *(((size_t *)opts->_group_table[opt->group - 1]) - 1) = count;
      opts->_group_table[opt->group - 1][count - 1] = opt;
    }
#endif // STARK_CLI_OPTS_ENABLE_HEAP
  }

  opts->_flags |= FLAG_GPS_DIRTY;

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  opts->_token_pool = malloc(
#ifdef ARG_MAX
      ARG_MAX
#else // ARG_MAX
#ifdef _WIN32
      UINT16_MAX
#else  // _WIN32
      STARK_CLI_OPTS_ARG_MAX_FALLBACK
#endif // _WIN32
#endif // ARG_MAX
  );

  if (opts->_token_pool == NULL) {
    cli_opts_error(NULL, 0, "stark_cli_opts_parse",
                   "allocation failed for token pool");

    return false;
  }

  opts->_token = opts->_token_pool;
  opts->_flags |= FLAG_TP_DIRTY;

#ifdef STARK_CLI_OPTS_ENABLE_ENV
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
    cli_opts_error(
        NULL, 0, "stark_cli_opts_parse",
        "unsupported operating system for environment variable parsing; "
        "remove STARK_CLI_OPTS_ENABLE_ENV");

    return false;
  }

  for (; *envp != NULL; envp++) {
    opts->_flags |= FLAG_VAL_TOKEN;

    strcpy(opts->_token, *envp);

    if (STARK_EXPECT_FALSE((opt = cli_opts_probe(opts, &opts->_env_lut,
                                                 LUT_TYPE_ENV)) != NULL)) {

      if (STARK_EXPECT_FALSE(!cli_opts_assign_opt(opts, opt))) {
        return false;
      }
    }
  }
#endif // STARK_CLI_OPTS_ENABLE_ENV
#endif // STARK_CLI_OPTS_ENABLE_HEAP

  size_t pos_idx = 0;
  bool eoo = false;

  for (opts->_argc = argc, opts->_argv = argv;;) {
    opts->_argc--;

    if (opts->_argc <= 0) {
      break;
    }

    opts->_argv++;
    opts->_flags |= FLAG_VAL_TOKEN;
    opt = NULL;

    if (STARK_EXPECT_FALSE((*opts->_argv) == NULL ||
                           (*opts->_argv)[0] == '\0')) {
      continue;
    }

    if (eoo || (*opts->_argv)[0] != '-' || (*opts->_argv)[1] == '\0') {
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
      strcpy(opts->_token, *opts->_argv);
#else  // STARK_CLI_OPTS_ENABLE_HEAP
      opts->_token = *opts->_argv;
#endif // STARK_CLI_OPTS_ENABLE_HEAP
      opt = !eoo ? cli_opts_probe(opts, &opts->_psc_lut, LUT_TYPE_PSC) : NULL;

      if (STARK_EXPECT_FALSE(opt == NULL)) {
        if (pos_idx == opts->_posc) {
          goto stark_cli_opts_parse_uopt;
        } else {
          opt = opts->_pos_lut[pos_idx];
        }
      } else {
        opt->_fstate = POSITIONAL;
      }

      opts->_flags |= FLAG_POS_OPT;

      if (STARK_EXPECT_FALSE(!cli_opts_assign_opt(opts, opt))) {
        return false;
      }

      if (opt->arr_count == opt->arr_len) {
        pos_idx++;
      }

      continue;
    }

    opts->_flags &= ~FLAG_POS_OPT;

    if ((*opts->_argv)[1] == '-') {
      goto stark_cli_opts_parse_lh;
    }

    bool mop;

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
    strcpy(opts->_token, &(*opts->_argv)[1]);
#else  // STARK_CLI_OPTS_ENABLE_HEAP
    opts->_token = &(*opts->_argv)[1];
#endif // STARK_CLI_OPTS_ENABLE_HEAP

    while (opts->_flags & FLAG_VAL_TOKEN) {
      if (STARK_EXPECT_FALSE(
              (opt = opts->_sh_lut[(unsigned char)opts->_token[0]]) == NULL)) {
        goto stark_cli_opts_parse_uopt;
      }

      if ((mop = opts->_token[1] != '\0')) {
        opts->_token = opts->_token + 1;
      } else {
        opts->_flags &= ~FLAG_VAL_TOKEN;
      }

      opt->_fstate = SHORTHAND;
      opts->_flags &= ~FLAG_LONG_OPT;

      if (STARK_EXPECT_FALSE(!cli_opts_assign_opt(opts, opt))) {
        return false;
      }

      if (!mop) {
        break;
      }
    }

    continue;

  stark_cli_opts_parse_lh:
    if ((*opts->_argv)[2] == '\0') {
      eoo = true;
      continue;
    }

    opts->_flags |= FLAG_LONG_OPT;

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
    strcpy(opts->_token, &(*opts->_argv)[2]);
#else  // STARK_CLI_OPTS_ENABLE_HEAP
    opts->_token = &(*opts->_argv)[2];
#endif // STARK_CLI_OPTS_ENABLE_HEAP

    if (STARK_EXPECT_FALSE((opt = cli_opts_probe(opts, &opts->_lh_lut,
                                                 LUT_TYPE_LH)) == NULL)) {
      goto stark_cli_opts_parse_uopt;
    }

    if (STARK_EXPECT_FALSE(!cli_opts_assign_opt(opts, opt))) {
      return false;
    }

    continue;

  stark_cli_opts_parse_uopt:
    cli_opts_error(opts, STARK_CLI_OPTS_ERR_UNKNOWN_OPT, NULL, opts->_token);

    return false;
  }

  for (uint32_t i = 0; i < opts->optc; i++) {
    opt = &opts->optv[i];

    if (opt->_fstate != NONE) {
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
      if (opt->group != 0) {
        for (size_t i = 0;
             i < *(((size_t *)opts->_group_table[opt->group - 1]) - 1); i++) {
          if (opts->_group_table[opt->group - 1][i] == opt) {
            continue;
          }

          char buf[STARK_CLI_OPTS_STRNAME_MAX * 2 + 22];
          int off = 0;

          switch (opts->_group_table[opt->group - 1][i]->_fstate) {
          case NONE:
#endif // STARK_CLI_OPTS_ENABLE_HEAP

            continue;

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
          case SHORTHAND:
            off = sprintf(buf, "'-%c' with option '-",
                          opts->_group_table[opt->group - 1][i]->shorthand);
            break;
          case LONGHAND:
            off = sprintf(buf, "'--%s' with option '--",
                          opts->_group_table[opt->group - 1][i]->longhand);
            break;
          case POSITIONAL:
            off = sprintf(buf, "'%s' with option '",
                          opts->_group_table[opt->group - 1][i]->longhand);
            break;
          case ENVIRONMENT:
            off = sprintf(buf, "'%s' with option '",
                          opts->_group_table[opt->group - 1][i]->env);
            break;
          }

          switch (opt->_fstate) {
          case SHORTHAND:
            sprintf(buf + off, "%c'", opt->shorthand);

            break;
          case POSITIONAL:
          case LONGHAND:
            sprintf(buf + off, "%s'", opt->longhand);

            break;
          case ENVIRONMENT:
            sprintf(buf + off, "%s'", opt->env);

            break;
          }

          cli_opts_error(opts, STARK_CLI_OPTS_ERR_CONFLICTING_OPT, NULL, buf);

          return false;
        }
      }
#endif // STARK_CLI_OPTS_ENABLE_HEAP
    } else if (opt->mods & STARK_CLI_OPT_MOD_REQUIRED) {
      char buf[STARK_CLI_OPTS_STRNAME_MAX + 3];

      sprintf(buf,
              opt->env != NULL || ((opt->mods & STARK_CLI_OPT_MOD_POSITIONAL) &&
                                   (opt->type == STARK_CLI_OPT_TYPE_SUBCOMMAND))
                  ? "%s"
              : opt->longhand != NULL ? "--%s"
                                      : "-%s",
              opt->env != NULL        ? opt->env
              : opt->longhand != NULL ? opt->longhand
                                      : (char[]){opt->shorthand, '\0'});

      cli_opts_error(opts, STARK_CLI_OPTS_ERR_MISSING_REQUIRED_OPT, NULL, buf);

      return false;
    }
  }

  return true;
}

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
void stark_cli_opts_free_token_pools(
    struct stark_cli_opts *const restrict opts) {
  free(opts->_token_pool);
  opts->_token_pool = NULL;
  opts->_token = NULL;
  opts->_flags &= ~FLAG_TP_DIRTY;

  for (uint32_t i = 0; i < opts->optc; i++) {
    if (opts->optv[i].type == STARK_CLI_OPT_TYPE_SUBCOMMAND &&
        (((struct stark_cli_opts *)opts->optv[i].ctx)->_flags &
         FLAG_TP_DIRTY)) {
      stark_cli_opts_free_token_pools(
          (struct stark_cli_opts *)opts->optv[i].ctx);
    }
  }
}

void stark_cli_opts_free_group_pools(
    struct stark_cli_opts *const restrict opts) {
  opts->_flags &= ~FLAG_GPS_DIRTY;

  for (uint8_t i = 0; i < UINT8_MAX; i++) {
    if (opts->_group_table[i] != NULL) {
      free(((size_t *)opts->_group_table[i]) - 1);
      opts->_group_table[i] = NULL;
    }
  }

  for (uint32_t i = 0; i < opts->optc; i++) {
    if (opts->optv[i].type == STARK_CLI_OPT_TYPE_SUBCOMMAND &&
        (((struct stark_cli_opts *)opts->optv[i].ctx)->_flags &
         FLAG_GPS_DIRTY)) {
      stark_cli_opts_free_group_pools(
          (struct stark_cli_opts *)opts->optv[i].ctx);
    }
  }
}
#endif // STARK_CLI_OPTS_ENABLE_HEAP

#undef FLAG_TP_DIRTY
#undef FLAG_GPS_DIRTY
#undef FLAG_INVALID
#undef FLAG_VERIFIED
#define INTERNAL_CLI_OPTS_UNDEF
#include "stark/internal/cli_opts.h"
#endif // STARK_CLI_OPTS_IMPL

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // STARK_CLI_OPTS_H
