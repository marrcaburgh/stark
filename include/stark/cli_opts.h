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
// Define STARK_CLI_OPTS_ENABLE_ENV to enable environment variable parsing if
// you're building for Unix-like operating systems, macOS, or Windows. See
// below.
//
// Define STARK_CLI_OPTS_ENABLE_HEAP to enable heap allocation for a worst-case
// sized token pool that is used to tokenize argv and slice delimited string
// arrays, and the environment if environment variable parsing is enabled.
//

#if defined(STARK_CLI_OPTS_ENABLE_ENV) && !defined(STARK_CLI_OPTS_ENABLE_HEAP)
#error "STARK_CLI_OPTS_ENABLE_ENV requires STARK_CLI_OPTS_ENABLE_HEAP"
#endif // STARK_CLI_OPTS_ENABLE_ENV && !STARK_CLI_OPTS_ENABLE_HEAP

#define STARK_CLI_OPTS_LONGHAND_MAX (512u)
#define STARK_CLI_OPTS_POSITIONAL_MAX (8u)
#define STARK_CLI_OPTS_POSITIONAL_SUBCOMMAND_MAX (512u)
#define STARK_CLI_OPTS_ENVIRONMENT_MAX (512u)
#define STARK_CLI_OPTS_GROUP_MAX (512u)

#include "stark/core.h"
#include "stark/hash_table.h"

#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define STARK_CLI_OPT_GROUP_DEP(g) (UINT64_C(1) << ((g) & 63u))

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum stark_cli_opts_err {
  /* function return codes */
  STARK_CLI_OPTS_ERR_SUCCESS = 0u,
  STARK_CLI_OPTS_ERR_FAILURE = 1u,
  STARK_CLI_OPTS_ERR_EXIT = 2u,

  /* builtin errors */
  STARK_CLI_OPTS_ERR_UNKNOWN_OPT = 3u,
  STARK_CLI_OPTS_ERR_UNEXPECTED_VAL = 4u,
  STARK_CLI_OPTS_ERR_NO_VAL = 5u,
  STARK_CLI_OPTS_ERR_OOB = 6u,
  STARK_CLI_OPTS_ERR_NAN = 7u,
  STARK_CLI_OPTS_ERR_OOR = 8u,
  STARK_CLI_OPTS_ERR_MISSING_REQUIRED_OPT = 9u,
  STARK_CLI_OPTS_ERR_CONFLICTING_OPT = 10u,
  STARK_CLI_OPTS_ERR_MISSING_OPT_DEPENDENCY = 11u,
} stark_cli_opts_err_t;

enum {
  /* C types */
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

  /* builtin types */
  STARK_CLI_OPT_TYPE_HELP = 14u,
  STARK_CLI_OPT_TYPE_VERSION = 15u
};

enum {
  STARK_CLI_OPT_MOD_GROUP = (1u << 0u),
  STARK_CLI_OPT_MOD_NO_GLOBALS = (1u << 1u),
  STARK_CLI_OPT_MOD_REQUIRED = (1u << 2u),
  STARK_CLI_OPT_MOD_POSITIONAL = (1u << 3u),
  STARK_CLI_OPT_MOD_ARRAY = (1u << 4u),
  STARK_CLI_OPT_MOD_HIDDEN = (1u << 5u),
  STARK_CLI_OPT_MOD_SHOW_DEFAULT = (1u << 6u),
};

typedef struct STARK_ALIGNED(64u) stark_cli_opt {
  uint32_t type : 4u;
  uint32_t mods : 7u;
  uint32_t cb_tag : 1u;
  uint32_t group : 9u;
  uint32_t _found_state : 3u;
  uint32_t _found_idx : 3u;
  uint32_t _pad0 : 5u;
  uint8_t _pad1;
  uint8_t arr_count;
  uint8_t const arr_len;
  unsigned char const shorthand;
  char const *const *const restrict longhand;
  char const *const *const restrict env;
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
  char const *const restrict desc;
} stark_cli_opt_t; // Fits into one 64-bit CPU's L1 cache line or 64 bytes of
                   // memory.

typedef struct stark_cli_opts {
  struct stark_hash_table _lh_lut, _pos_sbc_lut, _env_lut;
  struct stark_hash_table_bucket _lh_lut_bkts[STARK_CLI_OPTS_LONGHAND_MAX],
      _pos_sbc_lut_bkts[STARK_CLI_OPTS_POSITIONAL_SUBCOMMAND_MAX],
      _env_lut_bkts[STARK_CLI_OPTS_ENVIRONMENT_MAX];
  struct stark_cli_opt **_group_table[STARK_CLI_OPTS_GROUP_MAX],
      *_sh_lut[UCHAR_MAX],
      *_pos_lut[STARK_CLI_OPTS_POSITIONAL_MAX], *const optv, *goptv;
  char *_token_pool, *_token, **_argv;
  char const *const restrict name, *version, *desc, *footer;
  uint64_t const group_deps[8];
  int _argc;
  uint32_t _posc;
  uint16_t const optc;
  uint16_t goptc;
  uint8_t _flags;
  uint8_t _lh_idx_lut[STARK_CLI_OPTS_LONGHAND_MAX];
  uint8_t _env_idx_lut[STARK_CLI_OPTS_ENVIRONMENT_MAX];
  uint8_t _pos_sbc_idx_lut[STARK_CLI_OPTS_POSITIONAL_SUBCOMMAND_MAX];
} stark_cli_opts_t;

enum stark_cli_opts_err stark_cli_opts_parse(struct stark_cli_opts *opts,
                                             int argc, char **argv);
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
void stark_cli_opts_free_token_pools(struct stark_cli_opts *opts);
void stark_cli_opts_free_group_pools(struct stark_cli_opts *opts);
#endif // STARK_CLI_OPTS_ENABLE_HEAP

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef STARK_CLI_OPTS_IMPL
#define FLAG_TP_DIRTY (1u << 6u)
#define FLAG_GPS_DIRTY (1u << 7u)

#include "stark/internal/cli_opts.h"
#if defined(__APPLE__) && defined(__MACH__)
#include <crt_externs.h>
#endif // __APPLE__ && __MACH__

#if defined(__linux__) || defined(__linux)
#include <linux/limits.h>
#endif // __linux__ || __linux

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum stark_cli_opts_err
stark_cli_opts_parse(struct stark_cli_opts *const restrict opts, int const argc,
                     char **argv) {
  if (STARK_EXPECT_FALSE(opts == NULL)) {
    cli_opts_error(NULL, 0, "stark_cli_opts_parse", "opts cannot be NULL");

    return STARK_CLI_OPTS_ERR_FAILURE;
  } else if (STARK_EXPECT_FALSE(argc <= 0)) {
    cli_opts_error(NULL, 0, "stark_cli_opts_parse",
                   "argc must be greater than 0");

    return STARK_CLI_OPTS_ERR_FAILURE;
  } else if (STARK_EXPECT_FALSE(argv == NULL)) {
    cli_opts_error(NULL, 0, "stark_cli_opts_parse", "argv cannot be NULL");

    return STARK_CLI_OPTS_ERR_FAILURE;
  } else if (STARK_EXPECT_FALSE(!cli_opts_init(opts))) {
    return STARK_CLI_OPTS_ERR_FAILURE;
  }

  struct stark_cli_opt *opt;
  enum stark_cli_opts_err err;
  bool gmode = false;

stark_cli_opts_parse_reset:
  for (uint32_t i = 0; i < (gmode ? opts->goptc : opts->optc); i++) {
    opt = &(gmode ? opts->goptv : opts->optv)[i];
    opt->_found_state = FOUND_NONE;
    opt->arr_count = 0;

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
    if (!(opts->_flags & FLAG_GPS_DIRTY) &&
        (opt->mods & STARK_CLI_OPT_MOD_GROUP)) {
      size_t count = 1;

      if (opts->_group_table[opt->group] == NULL) {
        opts->_group_table[opt->group] = malloc(
            sizeof(size_t) + (opts->optc + opts->goptc) * sizeof(void *));

        if (opts->_group_table[opt->group] == NULL) {
          cli_opts_error(NULL, 0, "stark_cli_opts_parse",
                         "allocation for group pool failed");

          return STARK_CLI_OPTS_ERR_FAILURE;
        }

        opts->_group_table[opt->group] =
            (struct stark_cli_opt **)(((size_t *)
                                           opts->_group_table[opt->group]) +
                                      1);
      } else {
        count += *(((size_t *)opts->_group_table[opt->group]) - 1);
      }

      *(((size_t *)opts->_group_table[opt->group]) - 1) = count;
      opts->_group_table[opt->group][count - 1] = opt;
    }
#endif // STARK_CLI_OPTS_ENABLE_HEAP
  }

  if (opts->goptv != NULL && !gmode) {
    gmode = true;

    goto stark_cli_opts_parse_reset;
  }

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  opts->_flags |= FLAG_GPS_DIRTY;
#endif // STARK_CLI_OPTS_ENABLE_HEAP

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  size_t max =
#ifdef ARG_MAX
      ARG_MAX
#else // ARG_MAX
#ifdef _WIN32
      32768u
#else  // _WIN32
      STARK_CLI_OPTS_ARG_MAX_FALLBACK
#endif // _WIN32
#endif // ARG_MAX
      ;

#if defined(STARK_CLI_OPTS_ENABLE_ENV) && defined(_WIN32)
  bool allocated = false;

  goto stark_cli_opts_parse_winalloc;

stark_cli_opts_parse_winalloced:
  allocated = true;
#endif // STARK_CLI_OPTS_ENABLE_ENV && _WIN32

  if (!(opts->_flags & FLAG_TP_DIRTY) &&
      (opts->_token_pool = malloc(max)) == NULL) {
    cli_opts_error(NULL, 0, "stark_cli_opts_parse",
                   "allocation failed for token pool");

    return STARK_CLI_OPTS_ERR_FAILURE;
  }

  opts->_token = opts->_token_pool;
  opts->_flags |= FLAG_TP_DIRTY;

#if defined(STARK_CLI_OPTS_ENABLE_ENV) && defined(_WIN32)
stark_cli_opts_parse_winalloc:
#endif // STARK_CLI_OPTS_ENABLE_ENV && _WIN32

#ifdef STARK_CLI_OPTS_ENABLE_ENV
  char **envp;
#ifdef _WIN32
  extern char **_environ;
  envp = _environ;
#elif defined(__APPLE__) && defined(__MACH__)
  envp = *_NSGetEnviron();
#elif defined(__unix__) || defined(__unix)
  extern char **environ;
  envp = environ;
#else  // __unix__ || __unix
  envp = NULL;
#endif // _WIN32

  if (STARK_EXPECT_FALSE(envp == NULL)) {
    cli_opts_error(
        NULL, 0, "stark_cli_opts_parse",
        "unsupported operating system for environment variable parsing; "
        "remove STARK_CLI_OPTS_ENABLE_ENV");

    return STARK_CLI_OPTS_ERR_FAILURE;
  }

#if defined(STARK_CLI_OPTS_ENABLE_ENV) && defined(_WIN32)
  if (!allocated) {
    size_t envc = 0;

    for (; envp[envc] != NULL; envc++)
      ;

    max *= envc;

    goto stark_cli_opts_parse_winalloced;
  }
#endif // STARK_CLI_OPTS_ENABLE_ENV && _WIN32

  for (; *envp != NULL; envp++) {
    opts->_flags |= FLAG_VAL_TOKEN;

    strcpy(opts->_token, *envp);

    if (STARK_EXPECT_FALSE((opt = cli_opts_probe(opts, LUT_TYPE_ENV)) !=
                           NULL)) {
      if ((err = cli_opts_assign_opt(opts, opt)) !=
          STARK_CLI_OPTS_ERR_SUCCESS) {
        return err;
      }
    }
  }
#endif // STARK_CLI_OPTS_ENABLE_ENV
#endif // STARK_CLI_OPTS_ENABLE_HEAP

  size_t pos_idx = 0;
  bool end_of_opts = false;

  for (opts->_argc = argc, opts->_argv = argv;;) {
    opts->_argc--;

    if (opts->_argc <= 0) {
      break;
    }

    opts->_argv++;

    if (*opts->_argv == NULL || *(opts->_argv)[0] == '\0') {
      continue;
    }

    opts->_flags |= FLAG_VAL_TOKEN;

    if (end_of_opts || (*opts->_argv)[0] != '-' || (*opts->_argv)[1] == '\0') {
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
      strcpy(opts->_token, *opts->_argv);

#else  // STARK_CLI_OPTS_ENABLE_HEAP
      opts->_token = *opts->_argv;
#endif // STARK_CLI_OPTS_ENABLE_HEAP
      opt = !end_of_opts ? cli_opts_probe(opts, LUT_TYPE_POS_SBC) : NULL;

      if (STARK_EXPECT_FALSE(opt == NULL)) {
        if (pos_idx == opts->_posc) {
          goto stark_cli_opts_parse_uopt;
        } else {
          opt = opts->_pos_lut[pos_idx];
        }
      }

      opts->_flags |= FLAG_POS_OPT;

      if ((err = cli_opts_assign_opt(opts, opt)) !=
          STARK_CLI_OPTS_ERR_SUCCESS) {
        return err;
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

    bool multi_opts;

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
    strcpy(opts->_token, &(*opts->_argv)[1]);
#else  // STARK_CLI_OPTS_ENABLE_HEAP
    opts->_token = &(*opts->_argv)[1];
#endif // STARK_CLI_OPTS_ENABLE_HEAP

    while (opts->_flags & FLAG_VAL_TOKEN) {
      if (STARK_EXPECT_FALSE(
              (opt = opts->_sh_lut[(unsigned char)opts->_token[0] - 1]) ==
              NULL)) {
        goto stark_cli_opts_parse_uopt;
      }

      if ((multi_opts = opts->_token[1] != '\0')) {
        opts->_token = &opts->_token[1];
      } else {
        opts->_flags &= ~FLAG_VAL_TOKEN;
      }

      opt->_found_state = FOUND_SHORTHAND;
      opts->_flags &= ~FLAG_LONG_OPT;

      if ((err = cli_opts_assign_opt(opts, opt)) !=
          STARK_CLI_OPTS_ERR_SUCCESS) {
        return err;
      }

      if (!multi_opts) {
        break;
      }
    }

    continue;

  stark_cli_opts_parse_lh:
    if ((*opts->_argv)[2] == '\0') {
      end_of_opts = true;
      continue;
    }

    opts->_flags |= FLAG_LONG_OPT;

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
    strcpy(opts->_token, &(*opts->_argv)[2]);
#else  // STARK_CLI_OPTS_ENABLE_HEAP
    opts->_token = &(*opts->_argv)[2];
#endif // STARK_CLI_OPTS_ENABLE_HEAP

    if (STARK_EXPECT_FALSE((opt = cli_opts_probe(opts, LUT_TYPE_LH)) == NULL)) {
      goto stark_cli_opts_parse_uopt;
    }

    if ((err = cli_opts_assign_opt(opts, opt)) != STARK_CLI_OPTS_ERR_SUCCESS) {
      return err;
    }

    continue;

  stark_cli_opts_parse_uopt:
    cli_opts_error(opts, STARK_CLI_OPTS_ERR_UNKNOWN_OPT, NULL, opts->_token);

    return STARK_CLI_OPTS_ERR_FAILURE;
  }

  gmode = false;

stark_cli_opts_parse_post_check:
  for (uint32_t i = 0; i < (gmode ? opts->goptc : opts->optc); i++) {
    opt = &(gmode ? opts->goptv : opts->optv)[i];

    if (opt->_found_state != FOUND_NONE) {
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
      if (opt->mods & STARK_CLI_OPT_MOD_GROUP) {
        for (size_t j = 0;
             j < *(((size_t *)opts->_group_table[opt->group]) - 1); j++) {
          struct stark_cli_opt *gropt = opts->_group_table[opt->group][j];
          if (gropt == opt) {
            continue;
          }

          char buf[STARK_CLI_OPTS_STRNAME_MAX * 2 + 29];

          if (!GET_GROUP_DEP(opts->group_deps, opt->group)) {
            if (gropt->_found_state == FOUND_NONE) {
              continue;
            }

            char const *gname = gropt->_found_state == FOUND_SHORTHAND
                                    ? (char[]){(char)gropt->shorthand, '\0'}
                                : gropt->_found_state == FOUND_ENVIRONMENT
                                    ? gropt->env[gropt->_found_idx]
                                    : gropt->longhand[gropt->_found_idx];
            char const *oname = opt->_found_state == FOUND_SHORTHAND
                                    ? (char[]){(char)opt->shorthand, '\0'}
                                : opt->_found_state == FOUND_ENVIRONMENT
                                    ? opt->env[opt->_found_idx]
                                    : opt->longhand[opt->_found_idx];
            size_t glen = strlen(gname), olen = strlen(oname);

            if (glen > STARK_CLI_OPTS_STRNAME_MAX) {
              glen = STARK_CLI_OPTS_STRNAME_MAX;
            }

            if (olen > STARK_CLI_OPTS_STRNAME_MAX) {
              olen = STARK_CLI_OPTS_STRNAME_MAX;
            }

            sprintf(buf, "'%s%.*s' with option '%s%.*s'",
                    gropt->_found_state == FOUND_SHORTHAND  ? "-"
                    : gropt->_found_state == FOUND_LONGHAND ? "--"
                                                            : "",
                    (int)glen, gname,
                    opt->_found_state == FOUND_SHORTHAND  ? "-"
                    : opt->_found_state == FOUND_LONGHAND ? "--"
                                                          : "",
                    (int)olen, oname);

            cli_opts_error(opts, STARK_CLI_OPTS_ERR_CONFLICTING_OPT, NULL, buf);

            return STARK_CLI_OPTS_ERR_FAILURE;
          } else if (gropt->_found_state == FOUND_NONE) {
            bool is_long = (opt->_found_state == FOUND_POSITIONAL ||
                            opt->_found_state == FOUND_LONGHAND);
            bool is_env = (opt->_found_state == FOUND_ENVIRONMENT);

            cli_opts_error(opts, STARK_CLI_OPTS_ERR_MISSING_OPT_DEPENDENCY,
                           is_env ? gropt->env[0]
                           : gropt->longhand[0] != NULL
                               ? gropt->longhand[0]
                               : (char[]){(char)gropt->shorthand, '\0'},
                           is_env    ? opt->env[0]
                           : is_long ? opt->longhand[0]
                                     : (char[]){(char)opt->shorthand, '\0'});

            return STARK_CLI_OPTS_ERR_FAILURE;
          }
        }
      }
#endif // STARK_CLI_OPTS_ENABLE_HEAP
    } else if (opt->mods & STARK_CLI_OPT_MOD_REQUIRED) {
      cli_opts_error(opts, STARK_CLI_OPTS_ERR_MISSING_REQUIRED_OPT, NULL,
                     opt->env != NULL ? opt->env[0]
                     : opt->longhand != NULL
                         ? opt->longhand[0]
                         : (char[]){(char)opt->shorthand, '\0'});

      return STARK_CLI_OPTS_ERR_FAILURE;
    }
  }

  if (opts->goptv != NULL && !gmode) {
    gmode = true;

    goto stark_cli_opts_parse_post_check;
  }

  return STARK_CLI_OPTS_ERR_SUCCESS;
}

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
void stark_cli_opts_free_token_pools(
    struct stark_cli_opts *const restrict opts) {
  if (!(opts->_flags & FLAG_TP_DIRTY)) {
    return;
  }

  opts->_flags &= ~FLAG_TP_DIRTY;

  free(opts->_token_pool);

  for (uint32_t i = 0; i < opts->optc; i++) {
    if (opts->optv[i].type == STARK_CLI_OPT_TYPE_SUBCOMMAND) {
      stark_cli_opts_free_token_pools(
          (struct stark_cli_opts *)opts->optv[i].ctx);
    }
  }
}

void stark_cli_opts_free_group_pools(
    struct stark_cli_opts *const restrict opts) {
  if (!(opts->_flags & FLAG_GPS_DIRTY)) {
    return;
  }

  opts->_flags &= ~FLAG_GPS_DIRTY;

  for (uint16_t i = 0; i < STARK_CLI_OPTS_GROUP_MAX; i++) {
    if (opts->_group_table[i] != NULL) {
      free(((size_t *)opts->_group_table[i]) - 1);
      opts->_group_table[i] = NULL;
    }
  }

  for (uint32_t i = 0; i < opts->optc; i++) {
    if (opts->optv[i].type == STARK_CLI_OPT_TYPE_SUBCOMMAND) {
      stark_cli_opts_free_group_pools(
          (struct stark_cli_opts *)opts->optv[i].ctx);
    }
  }
}
#endif // STARK_CLI_OPTS_ENABLE_HEAP

#undef FLAG_TP_DIRTY
#undef FLAG_GPS_DIRTY
#define INTERNAL_CLI_OPTS_UNDEF
#include "stark/internal/cli_opts.h"
#endif // STARK_CLI_OPTS_IMPL

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // STARK_CLI_OPTS_H
