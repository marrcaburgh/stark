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

#ifndef STARK_CLI_OPTS_H
#define STARK_CLI_OPTS_H

//
// Uncomment these to get syntax highlighting of the majority of the code in
// this header, the default (stack) parts will still be grayed out:
//
// #define STARK_CLI_OPTS_ENABLE_ENV
// #define STARK_CLI_OPTS_ENABLE_HEAP
// #define STARK_CLI_OPTS_IMPL
//

#include "stark/core.h"
#include "stark/hash_table.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define STARK_CLI_OPTS_OPT_MAX (1258291u + UINT8_MAX)

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
// argv point to mutable strings otherwise this is undefined behavior.
//
#ifndef STARK_CLI_OPTS_LH_LUT_SIZE
#define STARK_CLI_OPTS_LH_LUT_SIZE 64
#endif // STARK_CLI_OPTS_LH_LUT_SIZE
#ifndef STARK_CLI_OPTS_POS_LUT_SIZE
#define STARK_CLI_OPTS_POS_LUT_SIZE 4
#endif // STARK_CLI_OPTS_POS_LUT_SIZE
#ifndef STARK_CLI_OPTS_PSC_LUT_SIZE
#define STARK_CLI_OPTS_PSC_LUT_SIZE 8
#endif // STARK_CLI_OPTS_PSC_LUT_SIZE
#ifndef STARK_CLI_OPTS_ENV_LUT_SIZE
#define STARK_CLI_OPTS_ENV_LUT_SIZE 16
#endif // STARK_CLI_OPTS_ENV_LUT_SIZE

typedef enum stark_cli_opts_err {
  STARK_CLI_OPTS_ERR_UNKNOWN_OPTION = 1,
  STARK_CLI_OPTS_ERR_NO_VALUE = 2,
  STARK_CLI_OPTS_ERR_OOB = 3,
  STARK_CLI_OPTS_ERR_NAN = 4,
  STARK_CLI_OPTS_ERR_OOR = 5,
  STARK_CLI_OPTS_ERR_MISSING_REQUIRED_OPTION = 6,
  STARK_CLI_OPTS_ERR_CONFLICTING_OPTION = 7,
} stark_cli_opts_err_t;

enum {
  /* regular types */
  STARK_CLI_OPT_TYPE_BOOLEAN = 1,
  STARK_CLI_OPT_TYPE_INT64 = 2,
  STARK_CLI_OPT_TYPE_INT32 = 3,
  STARK_CLI_OPT_TYPE_INT16 = 4,
  STARK_CLI_OPT_TYPE_INT8 = 5,
  STARK_CLI_OPT_TYPE_UINT64 = 6,
  STARK_CLI_OPT_TYPE_UINT32 = 7,
  STARK_CLI_OPT_TYPE_UINT16 = 8,
  STARK_CLI_OPT_TYPE_UINT8 = 9,
  STARK_CLI_OPT_TYPE_FLOAT64 = 10,
  STARK_CLI_OPT_TYPE_FLOAT32 = 11,
  STARK_CLI_OPT_TYPE_STRING = 12,

  /* special types */
  STARK_CLI_OPT_TYPE_SUBCOMMAND = 13,

  /* bltn types */
  STARK_CLI_OPT_TYPE_HELP = 14,
};

enum {
  STARK_CLI_OPT_MOD_REQUIRED = (1 << 0),
  STARK_CLI_OPT_MOD_HIDDEN = (1 << 1),
  STARK_CLI_OPT_MOD_POSITIONAL = (1 << 2),
  STARK_CLI_OPT_MOD_ARRAY = (1 << 3),
};

typedef struct STARK_ALIGNED(64) stark_cli_opt {
  uint8_t type : 4;
  uint8_t mods : 4;
  uint8_t group : 5;
  uint8_t cb_tag : 1;
  uint8_t _fstate : 2;
  uint8_t arrc;
  uint8_t const arrl;
  char const delim;
  unsigned char const shorthand;
  uint8_t pad[2];
  char const *const restrict longhand;
  char const *const restrict env;
  void *const restrict dest;
  bool (*const assign)(char const *const restrict str,
                       void *const restrict dest, uint8_t const arrc,
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
  struct stark_cli_opt *const optv, **_group_table[63], *_sh_lut[256],
      *_pos_lut[STARK_CLI_OPTS_POS_LUT_SIZE];
  void (*const err_callback)(enum stark_cli_opts_err const errc,
                             char const *ctx);
  char *_token_pool, *_token, **_argv;
  char const *const restrict desc;
  int _argc;
  uint32_t const optc;
  uint8_t _flags;
  uint8_t _posc;
} stark_cli_opts_t;

STARK_COLD bool stark_cli_opts_init(struct stark_cli_opts *opts);
bool stark_cli_opts_parse(struct stark_cli_opts *opts, int argc, char **argv);
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
void stark_cli_opts_free_token_pool(struct stark_cli_opts *opts);
void stark_cli_opts_free_group_pools(struct stark_cli_opts *opts);
#endif // STARK_CLI_OPTS_ENABLE_HEAP

#ifdef STARK_CLI_OPTS_IMPL

#define STARK_INTERNAL_CLI_OPTS_DONT_UNDEF
#include "stark/internal/cli_opts.h"
#undef STARK_INTERNAL_CLI_OPTS_DONT_UNDEF

#define FLAG_DIRTY (1u << 3)
#define FLAG_INVALID (1u << 4)
#define FLAG_VERIFIED (1u << 5)

STARK_COLD bool
stark_cli_opts_init(struct stark_cli_opts *const restrict opts) {
  bool valid = true, vp = false;

  if (opts == NULL) {
    error(NULL, 0, "stark_cli_opts_init", "cli_opts cannot be NULL");

    return false;
  } else if (opts->optv == NULL) {
    error(NULL, 0, "stark_cli_opts_init", "optv cannot be NULL");

    return false;
  } else if (opts->optc <= 0) {
    error(NULL, 0, "stark_cli_opts_init", "optc must be greater than 1");

    return false;
  } else if (opts->optc > STARK_CLI_OPTS_OPT_MAX) {
    error(NULL, 0, "stark_cli_opts_init",
          "optc cannot be greater than %" PRIu32, STARK_CLI_OPTS_OPT_MAX);

    return false;
  } else if (opts->_flags & FLAG_INVALID) {
    return false;
  } else if (opts->_flags & FLAG_VERIFIED) {
    return true;
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
stark_cli_opts_init_loop:
  if (li == opts->optc) {
    goto stark_cli_opts_init_loope;
  }

  stark_cli_opt_t *const restrict opt = &opts->optv[li];

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  if (opt->group != 0) {
    size_t count = 1;

    if (opts->_group_table[opt->group - 1] == NULL) {
      opts->_group_table[opt->group - 1] = malloc(
          sizeof(size_t) + (STARK_CLI_OPTS_OPT_MAX + count++) * sizeof(void *));

      if (opts->_group_table[opt->group - 1] == NULL) {
        error(NULL, 0, "stark_cli_opts_init",
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
    opts->_group_table[opt->group - 1][count] = NULL;
  }
#endif // STARK_CLI_OPTS_ENABLE_HEAP

  if (opt->mods & STARK_CLI_OPT_MOD_ARRAY) {
    if (opt->arrl <= 1) {
      error(NULL, 0, "stark_cli_opts_init",
            "array options must have an arrl that is greater than 1");

      valid = false;
    }

    switch (opt->delim) {
    case ',':
    case ':':
    case ';':
      if (opt->type == STARK_CLI_OPT_TYPE_BOOLEAN) {
        error(NULL, 0, "stark_cli_opts_init",
              "boolean options cannot have delimiters");

        valid = false;
      }
      break;
    default:
      if (opt->type != STARK_CLI_OPT_TYPE_BOOLEAN
#ifndef STARK_CLI_OPTS_ENABLE_HEAP
          && opt->type != STARK_CLI_OPT_TYPE_STRING
#endif // STARK_CLI_OPTS_ENABLE_HEAP
      ) {
        error(NULL, 0, "stark_cli_opts_init",
              "missing delimiter for array option (expected one of: ',', ':', "
              "';')");

        valid = false;
      }

      break;
    }
  }

  if (opt->assign != NULL) {
    if (opt->dest == NULL) {
      error(NULL, 0, "stark_cli_opts_init",
            "option missing destination pointer");

      valid = false;
    }

    if (opt->type != 0) {
      error(NULL, 0, "stark_cli_opts_init",
            "assigners cannot be combined with a type");

      valid = false;
    }

    if (opt->callback.callback != NULL && opt->cb_tag == 0) {
      error(NULL, 0, "stark_cli_opts_init",
            "assigners cannot be combined with callbacks");

      valid = false;
    }

    goto stark_cli_opts_init_skip_bltn;
  } else if (opt->callback.callback != NULL && opt->cb_tag == 0) {
    goto stark_cli_opts_init_skip_bltn;
  }

  switch (opt->type) {
  case STARK_CLI_OPT_TYPE_STRING:
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
  case STARK_CLI_OPT_TYPE_BOOLEAN:
    if (opt->dest == NULL) {
      error(NULL, 0, "stark_cli_opts_init",
            "option missing destination pointer");

      valid = false;
    }

    break;
  case STARK_CLI_OPT_TYPE_SUBCOMMAND:
    if (opt->ctx == NULL) {
      error(NULL, 0, "stark_cli_opts_init",
            "subcommand option missing context");

      valid = false;
    }

    break;
  case STARK_CLI_OPT_TYPE_HELP:
    break;
  }

stark_cli_opts_init_skip_bltn:
  if (opt->mods & STARK_CLI_OPT_MOD_POSITIONAL) {
    if (opt->type == STARK_CLI_OPT_TYPE_SUBCOMMAND) {
      if (opt->longhand == NULL) {
        error(NULL, 0, "stark_cli_opts_init",
              "positional subcommand options must have a longhand");

        valid = false;
      } else {
        valid &= lut_insert(opt, &opts->_psc_lut, LUT_TYPE_PSC);
      }

      goto stark_cli_opts_init_skip_rpos;
    } else if (opt->type == STARK_CLI_OPT_TYPE_BOOLEAN) {
      error(NULL, 0, "stark_cli_opts_init",
            "positional option modifier cannot be combined with boolean "
            "type");

      valid = false;
    }

    if (opts->_posc == STARK_CLI_OPTS_POS_LUT_SIZE) {
      error(NULL, 0, "stark_cli_opts_init",
            "positional option count exceeds limit; define "
            "STARK_CLI_OPTS_POS_LUT_SIZE before inclusion with a greater limit "
            "or "
            "remove "
            "options");

      valid = false;
    } else {
      if (vp) {
        error(NULL, 0, "stark_cli_opts_init",
              "variadic positional options must be the last positional");

        valid = false;
      }

      vp = (opt->mods & STARK_CLI_OPT_MOD_ARRAY);
      opts->_pos_lut[opts->_posc++] = opt;
    }

  stark_cli_opts_init_skip_rpos:
    goto stark_cli_opts_init_skip_onc;
  } else if (opt->shorthand == '\0' && opt->longhand == NULL) {
    error(NULL, 0, "stark_cli_opts_init",
          "non-positional options must have either a shorthand "
          "or longhand");

    valid = false;
  }

  if (opt->shorthand == '\0') {
    goto stark_cli_opts_init_skip_sh;
  }

  if (opts->_sh_lut[opt->shorthand] == NULL) {
    opts->_sh_lut[opt->shorthand] = opt;
  } else {
    error(NULL, 0, "stark_cli_opts_init", "duplicate shorthand option '%c'",
          opt->shorthand);

    valid = false;
  }

stark_cli_opts_init_skip_sh:
  if (opt->longhand != NULL) {
    valid &= lut_insert(opt, &opts->_lh_lut, LUT_TYPE_LH);
  }

#ifdef STARK_CLI_OPTS_ENABLE_ENV
  if (opt->env != NULL) {
    valid &= lut_insert(opt, &opts->_env_lut, LUT_TYPE_ENV);

    if (opt->mods & STARK_CLI_OPT_MOD_REQUIRED &&
        (opt->longhand != NULL || opt->shorthand != '\0')) {
      error(NULL, 0, "stark_cli_opts_init",
            "required options cannot have both env and longhand/shorthand");

      valid = false;
    }
  }
#endif // STARK_CLI_OPTS_ENABLE_ENV

stark_cli_opts_init_skip_onc:
  li++;

  goto stark_cli_opts_init_loop;

stark_cli_opts_init_loope:
  opts->_flags |= (valid ? FLAG_VERIFIED : FLAG_INVALID);

  return valid;
}

bool stark_cli_opts_parse(struct stark_cli_opts *const restrict opts,
                          int const argc, char **argv) {
  if (STARK_EXPECT_FALSE(opts == NULL)) {
    error(NULL, 0, "stark_cli_opts_parse", "cli_opts cannot be NULL");

    return false;
  } else if (STARK_EXPECT_FALSE(argv == NULL)) {
    error(NULL, 0, "stark_cli_opts_parse", "argv cannot be NULL");

    return false;
  } else if (STARK_EXPECT_FALSE(argc <= 0)) {
    error(NULL, 0, "stark_cli_opts_parse", "argc must be greater than 1");

    return false;
  } else if (STARK_EXPECT_FALSE(!(opts->_flags & FLAG_VERIFIED))) {
    error(NULL, 0, "stark_cli_opts_parse",
          "not verified; did you forget "
          "'stark_cli_opts_init()'?");

    return false;
  }
#ifdef STARK_CLI_OPTS_ENABLE_HEAP
  else if (STARK_EXPECT_FALSE(((opts->_flags & FLAG_DIRTY) &&
                               opts->_token_pool != NULL &&
                               opts->_token != NULL))) {
    error(NULL, 0, "stark_cli_opts_parse",
          "dirty; did you forget 'stark_cli_opts_free_token_pool()' or "
          "'stark_cli_opts_free_group_pools()'?");

    return false;
  }
#endif // STARK_CLI_OPTS_ENABLE_HEAP

  struct stark_cli_opt *opt;

  for (uint32_t i = 0; i < opts->optc; i++) {
    opt = &opts->optv[i];
    opt->_fstate = NONE;
    opt->arrc = 0;
  }

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
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
    error(NULL, 0, "stark_cli_opts_parse", "allocation failed for token pool");

    return false;
  }

  opts->_token = opts->_token_pool;
  opts->_flags |= FLAG_DIRTY;

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
    error(NULL, 0, "stark_cli_opts_parse",
          "unsupported operating system for environment variable parsing; "
          "remove STARK_CLI_OPTS_ENABLE_ENV");

    return false;
  }

  for (; *envp != NULL; envp++, opts->_flags |= FLAG_VALUE_TOKEN) {
    strcpy(opts->_token, *envp);

    if (STARK_EXPECT_FALSE((opt = probe(opts, &opts->_env_lut, LUT_TYPE_ENV)) !=
                           NULL)) {

      if (STARK_EXPECT_FALSE(!assign_opt(opts, opt))) {
        return false;
      }
    }
  }
#endif // STARK_CLI_OPTS_ENABLE_ENV
#endif // STARK_CLI_OPTS_ENABLE_HEAP

  size_t pos_idx = 0;
  bool eoo = false;

  for (opts->_argc = argc - 1, opts->_argv = argv + 1, opt = NULL,
      opts->_flags |= FLAG_VALUE_TOKEN;
       opts->_argc > 0; opts->_argc--, opts->_argv++, opt = NULL,
      opts->_flags |= FLAG_VALUE_TOKEN) {
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
      opt = !eoo ? probe(opts, &opts->_psc_lut, LUT_TYPE_PSC) : NULL;

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

      if (STARK_EXPECT_FALSE(!assign_opt(opts, opt))) {
        return false;
      }

      if (opt->arrc == opt->arrl) {
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

    while (opts->_flags & FLAG_VALUE_TOKEN) {
      if (STARK_EXPECT_FALSE(
              (opt = opts->_sh_lut[(unsigned char)opts->_token[0]]) == NULL)) {
        goto stark_cli_opts_parse_uopt;
      }

      if ((mop = opts->_token[1] != '\0')) {
        opts->_token = opts->_token + 1;
      } else {
        opts->_flags &= ~FLAG_VALUE_TOKEN;
      }

      opt->_fstate = SHORTHAND;
      opts->_flags &= ~FLAG_LONG_OPT;

      if (STARK_EXPECT_FALSE(!assign_opt(opts, opt))) {
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

    if (STARK_EXPECT_FALSE((opt = probe(opts, &opts->_lh_lut, LUT_TYPE_LH)) ==
                           NULL)) {
      goto stark_cli_opts_parse_uopt;
    }

    if (STARK_EXPECT_FALSE(!assign_opt(opts, opt))) {
      return false;
    }

    continue;

  stark_cli_opts_parse_uopt:
    error(opts, STARK_CLI_OPTS_ERR_UNKNOWN_OPTION, NULL, opts->_token);

    return false;
  }

  for (uint32_t i = 0; i < opts->optc; i++) {
    opt = &opts->optv[i];

    if (opt->_fstate != NONE) {
      if (opt->group != 0) {
        for (struct stark_cli_opt **oop = opts->_group_table[opt->group - 1];
             *oop != NULL; oop++) {
          if (*oop == opt) {
            continue;
          }

          char buf[UINT8_MAX * 2 + 22];
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

          switch (opt->_fstate) {
          case SHORTHAND:
            sprintf(buf + off, "%c'", opt->shorthand);
            break;
          case POSITIONAL:
          case LONGHAND:
            sprintf(buf + off, "%s'", opt->longhand);
            break;
          }

          error(opts, STARK_CLI_OPTS_ERR_CONFLICTING_OPTION, NULL, buf);

          return false;
        }
      }
    } else if (opt->mods & STARK_CLI_OPT_MOD_REQUIRED) {
      char buf[UINT8_MAX + 3];

      sprintf(buf,
              opt->env != NULL || ((opt->mods & STARK_CLI_OPT_MOD_POSITIONAL) &&
                                   (opt->type == STARK_CLI_OPT_TYPE_SUBCOMMAND))
                  ? "%s"
              : opt->longhand != NULL ? "--%s"
                                      : "-%s",
              opt->env != NULL        ? opt->env
              : opt->longhand != NULL ? opt->longhand
                                      : (char[]){opt->shorthand, '\0'});

      error(opts, STARK_CLI_OPTS_ERR_MISSING_REQUIRED_OPTION, NULL, buf);

      return false;
    }
  }

  return true;
}

#ifdef STARK_CLI_OPTS_ENABLE_HEAP
void stark_cli_opts_free_token_pool(
    struct stark_cli_opts *const restrict opts) {
  free(opts->_token_pool);
  opts->_token_pool = NULL;
  opts->_token = NULL;
  opts->_flags &= ~FLAG_DIRTY;
}

void stark_cli_opts_free_group_pools(
    struct stark_cli_opts *const restrict opts) {
  for (uint8_t i = 0; i < 63; i++) {
    free(((size_t *)opts->_group_table[i]) - 1);
    opts->_group_table[i] = NULL;
  }
}
#endif // STARK_CLI_OPTS_ENABLE_HEAP

#undef LUT_TYPE_LH
#undef LUT_TYPE_PSC
#undef LUT_TYPE_ENV
#undef FLAG_VALUE_TOKEN
#undef FLAG_LONG_OPT
#undef FLAG_POS_OPT
#undef FLAG_DIRTY
#undef FLAG_INVALID
#undef FLAG_VERIFIED
#endif // STARK_CLI_OPTS_IMPL

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // STARK_CLI_OPTS_H
