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
// lookup tables, define these macros before including this header.
#ifndef STARK_OPTS_LH_LUT_SIZE
#define STARK_OPTS_LH_LUT_SIZE 128
#endif
#ifndef STARK_OPTS_POS_LUT_SIZE
#define STARK_OPTS_POS_LUT_SIZE 4
#endif
#ifndef STARK_OPTS_PSC_LUT_SIZE
#define STARK_OPTS_PSC_LUT_SIZE 4
#endif

typedef enum stark_opts_err_type {
  STARK_OPTS_ERR_UNKNOWN_OPTION,
  STARK_OPTS_ERR_NO_VALUE,
  STARK_OPTS_ERR_OOB,
  STARK_OPTS_ERR_NAN,
  STARK_OPTS_ERR_OOR,
  STARK_OPTS_ERR_MISSING_QUOTE,
  STARK_OPTS_ERR_REQUIRED_NOT_FOUND,
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
  char delim;                    // 1 byte
  unsigned char const shorthand; // 1 byte
  uint8_t _long_len;             // 1 byte
  uint8_t _alias_len;            // 1 byte
  char const *const restrict longhand; // 8 bytes
  char const *const restrict alias;    // 8 bytes
  void *const restrict dest;           // 8 bytes
  bool (*const assign)(char const *const restrict str,
                       void *const restrict dest, uint8_t const arrc,
                       void **val_ptr);
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
  struct stark_opt *_psc_lut[STARK_OPTS_PSC_LUT_SIZE];
  char const *restrict _token;
  char const **_argv;
  struct stark_opt *const restrict optv;
  char const *const restrict desc;
  void (*const err_callback)(stark_opts_err_type const errc, char const *ctx);
  int _argc;
  int const optc;
  uint8_t _posc;
  bool _verified;
} stark_opts_t;

STARK_COLD bool stark_opts_init(struct stark_opts *const restrict opts);
bool stark_opts_parse(struct stark_opts *const restrict opts, int const argc,
                      char const **const argv);

#define STARK_OPTS_IMPL
#ifdef STARK_OPTS_IMPL

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STARK_OPT_UNKNOWN 2
#define STARK_OPT_ASSIGN_FAILED 1

STARK_COLD STARK_ALWAYS_INLINE STARK_UNUSED static inline void
usage(STARK_UNUSED const struct stark_opt *const restrict opt) {
  // TODO: print usage
}

STARK_COLD STARK_ALWAYS_INLINE static inline void
help(STARK_UNUSED struct stark_opts *const restrict opts) {
  // STARK_UNUSED stark_opt_callback const cb) {
  // TODO: print help
}

STARK_NOINLINE static bool
run_subcommand(struct stark_opts *const restrict opts,
               struct stark_opt *const restrict opt) {
  return stark_opts_parse(opt->ctx, opts->_argc, opts->_argv);
}

STARK_ALWAYS_INLINE static inline bool
assign_opt(struct stark_opts *const restrict opts,
           struct stark_opt *const restrict opt) {
  opt->mods |= STARK_OPT_FOUND;

  if (opt->type == STARK_OPT_TYPE_HELP) {
    help(opts);

    return true;
  } else if (opt->handler.callback != NULL) {
    opt->handler.callback(opt->ctx);

    return true;
  }

  char const *str;
  char *delim = NULL;
  void *val_ptr = NULL;

  if (opt->type == STARK_OPT_TYPE_SUBCOMMAND) {
    if (STARK_EXPECT_FALSE(!run_subcommand(opts, opt))) {
      return false;
    }

    opts->_argc = 0;
    return true;
  }

assign_opt_continue_array:
  if (delim != NULL) {
    opts->_token = delim + 1;
    *delim = opt->delim;
    delim = NULL;
  }

  if ((opt->mods & STARK_OPT_MOD_ARRAY) &&
      STARK_EXPECT_FALSE(opt->arrc >= opt->arrl)) {
    if (opts->err_callback) {
      opts->err_callback(STARK_OPTS_ERR_OOB, opts->_token);
    }

    return false;
  }

  if (opt->mods & STARK_OPT_MOD_POSITIONAL) {
    str = *opts->_argv;
  } else if (opt->type == STARK_OPT_TYPE_BOOLEAN) {
    goto assign_opt_skip_arg;

    // This is how you get `-i3` and '-i 3' to work properly, same with
    // `--int=3` vs `--int 3`.
  } else if (opts->_token != NULL && *opts->_token != '\0') {
    str = opts->_token;
    opts->_token = NULL;
  } else if (opts->_argc > 1) {
  assign_opt_retry_narg:
    opts->_argc--;
    opts->_argv++;

    if (opts->_argc <= 0) {
      return false;
    } else if (opts->_argv == NULL || *opts->_argv[0] == '\0') {
      goto assign_opt_retry_narg;
    }

    str = *opts->_argv;
  } else {
    if (opts->err_callback) {
      opts->err_callback(STARK_OPTS_ERR_NO_VALUE,
                         opts->_token != NULL ? opts->_token : *opts->_argv);
    }

    return false;
  }

  if (opt->assign != NULL) {
    if (STARK_EXPECT_FALSE(!opt->assign(str, opt->dest, opt->arrc, &val_ptr))) {
      return false;
    }
  } else if (opt->type == STARK_OPT_TYPE_BOOLEAN) {
  assign_opt_skip_arg:
    *(((bool *)opt->dest) + opt->arrc) = !(opt->mods & STARK_OPT_MOD_SET_FALSE);
  } else {

    union {
      long l;
      double d;
    } val;
    char *endptr = NULL;
    errno = 0;

    if (opt->type == STARK_OPT_TYPE_INTEGER ||
        opt->type == STARK_OPT_TYPE_LONG) {
      val.l = strtol(str, &endptr, 10);
    } else if (opt->type == STARK_OPT_TYPE_FLOAT ||
               opt->type == STARK_OPT_TYPE_DOUBLE) {
      val.d = strtod(str, &endptr);
    } else {
      goto assign_opt_skip_num;
    }

    if (STARK_EXPECT_FALSE(endptr == str)) {
      if (opts->err_callback) {
        opts->err_callback(STARK_OPTS_ERR_NAN, str);
      }

      return false;
    } else if (STARK_EXPECT_FALSE(errno == ERANGE)) {
      if (opts->err_callback) {
        opts->err_callback(STARK_OPTS_ERR_OOR, str);
      }

      return false;
    }

  assign_opt_skip_num:
    switch (opt->type) {
    case STARK_OPT_TYPE_INTEGER:
      if (STARK_EXPECT_FALSE(val.l > INT_MAX || val.l < INT_MIN)) {
        if (opts->err_callback) {
          opts->err_callback(STARK_OPTS_ERR_OOR, str);
        }

        return false;
      }

      *(((int *)opt->dest) + opt->arrc) = (int)val.l;
      val_ptr = ((int *)opt->dest) + opt->arrc;

      break;
    case STARK_OPT_TYPE_LONG:
      *(((long *)opt->dest) + opt->arrc) = val.l;
      val_ptr = ((long *)opt->dest) + opt->arrc;

      break;
    case STARK_OPT_TYPE_FLOAT:
      *(((float *)opt->dest) + opt->arrc) = (float)val.d;
      val_ptr = ((float *)opt->dest) + opt->arrc;

      break;
    case STARK_OPT_TYPE_DOUBLE:
      *(((double *)opt->dest) + opt->arrc) = val.d;
      val_ptr = ((double *)opt->dest) + opt->arrc;

      break;
    case STARK_OPT_TYPE_STRING:
      *(((char const **)opt->dest) + opt->arrc) = str;
      val_ptr = ((char const **)opt->dest) + opt->arrc;

      break;
    }
  }

  if (val_ptr != NULL && opt->handler.validate != NULL &&
      STARK_EXPECT_FALSE(!opt->handler.validate(val_ptr, opt->ctx))) {
    return false;
  }

  if ((opt->mods & STARK_OPT_MOD_ARRAY)) {
    opt->arrc++;

    if (opt->delim && !(opt->type == STARK_OPT_TYPE_BOOLEAN)) {
      char *oq, *tq = NULL;
      char q;

      if ((oq = strpbrk(str, "\"'")) != NULL) {
        q = *oq;
      } else {
        goto assign_opt_skip_quote;
      }

      tq = strchr(oq + 1, q);

      while (true) {
        if (tq == NULL) {
          if (opts->err_callback != NULL) {
            opts->err_callback(STARK_OPTS_ERR_MISSING_QUOTE, str);
          }

          return false;
        }

        bool esc = false;

        for (size_t i = 1; tq - i > oq; i++, esc = !esc) {
          if (tq[-i] != '\\') {
            break;
          }
        }

        if (!esc) {
          break;
        }

        tq = strchr(tq + 1, q);
      }

    assign_opt_skip_quote:
      delim = strchr(tq != NULL ? tq + 1 : str, opt->delim);

      if (delim != NULL) {
        *delim = '\0';
        goto assign_opt_continue_array;
      }
    }
  }

  return true;
}

STARK_ALWAYS_INLINE static inline uint32_t hash(const char *restrict str) {
  uint32_t h = 2166136261u;

  while (*str != '\0') {
    h ^= (unsigned char)*str++;
    h *= 16777619u;
  }

  return h;
}

STARK_ALWAYS_INLINE static inline uint32_t hash_n(const char *restrict str,
                                                  size_t const n) {
  uint32_t h = 2166136261u;

  for (size_t i = 0; i < n; i++) {
    h ^= (unsigned char)*str++;
    h *= 16777619u;
  }

  return h;
}

STARK_ALWAYS_INLINE STARK_FLATTEN static inline int
match_longhand(struct stark_opts *const restrict opts) {
  struct stark_opt *restrict o;
  char const *const restrict token = opts->_token, *const restrict eq =
                                                       strchr(token, '=');
  size_t const t_len = eq != NULL ? (size_t)(eq - token) : strlen(token);
  size_t i = hash_n(token, t_len) & (STARK_OPTS_LH_LUT_SIZE - 1), probes = 0;

  while (true) {
    o = opts->_lh_lut[i];

    if (o == NULL) {
      return STARK_OPT_UNKNOWN;
    }

    if (STARK_EXPECT_TRUE(
            (o->longhand != NULL &&
             (STARK_EXPECT_TRUE(o->_long_len == t_len)) &&
             STARK_EXPECT_TRUE(memcmp(o->longhand, token, t_len) == 0)) ||
            (o->alias != NULL && STARK_EXPECT_TRUE(o->_alias_len == t_len) &&
             STARK_EXPECT_TRUE(memcmp(o->alias, token, t_len) == 0)))) {
      break;
    }

    if (STARK_EXPECT_FALSE(++probes == STARK_OPTS_LH_LUT_SIZE)) {
      return STARK_OPT_UNKNOWN;
    }

    i = (i + 1) & (STARK_OPTS_LH_LUT_SIZE - 1);
  }

  opts->_token = eq != NULL ? eq + 1 : NULL;

  return STARK_EXPECT_TRUE(assign_opt(opts, o)) ? 0 : STARK_OPT_ASSIGN_FAILED;
}

STARK_ALWAYS_INLINE STARK_FLATTEN static inline int
match_shorthand(struct stark_opts *const restrict opts) {
  struct stark_opt *restrict o;
  bool const combined = opts->_token[1] != '\0';

  while (opts->_token != NULL) {
    o = opts->_sh_lut[(unsigned char)*opts->_token];

    if (STARK_EXPECT_FALSE(o == NULL)) {
      return STARK_OPT_UNKNOWN;
    }

    opts->_token = opts->_token[1] != '\0' ? &opts->_token[1] : NULL;

    if (STARK_EXPECT_FALSE(!assign_opt(opts, o))) {
      return STARK_OPT_ASSIGN_FAILED;
    }

    if (!combined) {
      break;
    }
  }

  return 0;
}

STARK_COLD static void error(const char *const errstr, ...) {
  va_list ap;

  va_start(ap, errstr);
  fprintf(stderr, "stark_opts error: ");
  vfprintf(stderr, errstr, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

STARK_COLD STARK_ALWAYS_INLINE STARK_FLATTEN static inline bool
register_opt(struct stark_opts *const restrict opts,
             struct stark_opt *const restrict opt) {
  bool ok = true;

  if (opt->mods & STARK_OPT_MOD_ARRAY) {
    switch (opt->delim) {
    case '\0':
      opt->delim = ',';
    case ',':
    case ':':
    case ';':
      break;
    default:
      error("register_opt(): delimiter must be one of the following: (',', "
            "':', ';')");

      ok = false;

      break;
    }
  }

  if (opt->assign != NULL) {
    if (opt->dest == NULL) {
      error("register_opt(): option missing destination pointer");

      ok = false;
    }

    goto register_opt_skip_builtin;
  } else if (opt->handler.callback != NULL) {
    goto register_opt_skip_builtin;
  }

  switch (opt->type) {
  case STARK_OPT_TYPE_STRING:
  case STARK_OPT_TYPE_DOUBLE:
  case STARK_OPT_TYPE_FLOAT:
  case STARK_OPT_TYPE_LONG:
  case STARK_OPT_TYPE_INTEGER:
  case STARK_OPT_TYPE_BOOLEAN:
    if (opt->dest == NULL) {
      error("register_opt(): option missing destination pointer");

      ok = false;
    }

    break;
  case STARK_OPT_TYPE_SUBCOMMAND:
    if (opt->ctx == NULL) {
      error("register_opt(): subcommand option missing context");

      ok = false;
    }

    break;
  case STARK_OPT_TYPE_HELP:
    break;
  default:
    error("register_opt(): invalid option type");

    ok = false;
  }
register_opt_skip_builtin:
  if (opt->mods & STARK_OPT_MOD_POSITIONAL) {
    if (opt->type == STARK_OPT_TYPE_SUBCOMMAND) {
      if (opt->usage == NULL) {
        error("register_opt(): positional subcommands must have a usage "
              "(name)");

        ok = false;
      } else {
        opt->_long_len = strlen(opt->usage);
      }
    } else if (opt->type == STARK_OPT_TYPE_BOOLEAN) {
      error("register_opt(): positional mod cannot be combined with boolean "
            "type");

      ok = false;
    }

    if (opts->_posc == STARK_OPTS_POS_LUT_SIZE) {
      error("register_opt(): positional argument count exceeds limit; define "
            "STARK_OPTS_POS_LUT_SIZE before inclusion with a greater limit or "
            "remove "
            "options");

      ok = false;
    } else {
      opts->_pos_lut[opts->_posc++] = opt;
    }

    return ok;
  } else if (opt->shorthand == '\0' && opt->longhand == NULL) {
    error("register_opt(): non-positional options must have either a shorthand "
          "or longhand");

    ok = false;
  }

  if (opt->shorthand == '\0') {
    goto register_opt_skip_shorthand;
  }

  if (opts->_sh_lut[opt->shorthand] == NULL) {
    opts->_sh_lut[opt->shorthand] = opt;
  } else {
    error("register_opt(): duplicate shorthand '-%c'", opt->shorthand);

    ok = false;
  }

register_opt_skip_shorthand:
  if (opt->longhand == NULL) {
    goto register_opt_skip_longhand;
  }

  size_t i = hash(opt->longhand) & (STARK_OPTS_LH_LUT_SIZE - 1), probes = 0,
         len = 0;

  if ((len = strlen(opt->longhand)) > 255) {
    error("register_opt(): longhand '--%s' length is greater than 255",
          opt->longhand);

    ok = false;
  } else {
    opt->_long_len = (uint8_t)len;
  }

  while (opts->_lh_lut[i] != NULL) {
    struct stark_opt const *prev = opts->_lh_lut[i];

    if (prev->longhand != NULL && strcmp(prev->longhand, opt->longhand) == 0) {
      error("register_opt(): duplicate longhand '--%s'", opt->longhand);
      ok = false;
    }

    if (++probes == STARK_OPTS_LH_LUT_SIZE) {
      error("register_opt(): longhand lookup table is full; define "
            "STARK_OPTS_LH_LUT_SIZE before "
            "inclusion with a greater limit or remove options");

      ok = false;
      break;
    }

    i = (i + 1) & (STARK_OPTS_LH_LUT_SIZE - 1);
  }

  opts->_lh_lut[i] = opt;

register_opt_skip_longhand:
  return ok;
}

STARK_COLD bool stark_opts_init(struct stark_opts *const restrict opts) {
  bool ok = true;

  if (opts == NULL) {
    error("stark_opts_init(): opts cannot be NULL");

    return false;
  } else if (opts->optv == NULL) {
    error("stark_opts_init(): optv cannot be NULL");

    return false;
  } else if (opts->optc <= 0) {
    error("stark_opts_init(): optc must be greater than zero");

    return false;
  } else if (!(ok = STARK_OPTS_LH_LUT_SIZE != 0 &&
                    ((STARK_OPTS_LH_LUT_SIZE & (STARK_OPTS_LH_LUT_SIZE - 1)) ==
                     0))) {
    error("stark_opts_init(): STARK_OPTS_LH_LUT_SIZE must be a power of two");
  } else if (opts->_verified) {
    return true;
  }

  for (int i = 0; i < opts->optc; i++) {
    struct stark_opt *const restrict o = &opts->optv[i];

    ok &= register_opt(opts, o);
  }

  return opts->_verified = ok;
}

bool stark_opts_parse(struct stark_opts *const restrict opts, int const argc,
                      char const **const restrict argv) {
  if (STARK_EXPECT_FALSE(!opts->_verified)) {
    error("stark_opts_parse(): not verified; did you forget "
          "'stark_opts_init()'?");

    return false;
  }

  uint8_t pos_idx = 0;
  bool array_pos = false, nop = false;

  opts->_argc = argc - 1;
  opts->_argv = argv + 1;

  for (int i = 0; i < opts->optc; i++) {
    struct stark_opt *o = &opts->optv[i];

    o->mods &= ~STARK_OPT_FOUND;
    o->arrc = 0;
  }

  for (; opts->_argc > 0; opts->_argc--, opts->_argv++) {
    char const *arg = *opts->_argv;

    if (arg == NULL || arg[0] == '\0') {
      continue;
    }

    if (nop || array_pos || arg[0] != '-' || arg[1] == '\0') {
      if (STARK_EXPECT_FALSE(opts->_posc == 0 || pos_idx >= opts->_posc)) {
        goto stark_opts_parse_unknown_option;
      }

      struct stark_opt *const restrict o = opts->_pos_lut[pos_idx];

      if (nop == false && o->type == STARK_OPT_TYPE_SUBCOMMAND) {
        size_t arg_len = strlen(arg);

        if (STARK_EXPECT_FALSE(!(
                o->_long_len == arg_len &&
                STARK_EXPECT_TRUE(memcmp(arg, o->usage, o->_long_len) == 0)))) {
          goto stark_opts_parse_unknown_option;
        }
      }

      if (STARK_EXPECT_TRUE(assign_opt(opts, o))) {
        if (o->mods & STARK_OPT_MOD_ARRAY) {
          array_pos = true;
        }
      } else {
        return false;
      }

      if (!array_pos) {
        pos_idx++;
      }

      continue;
    }

    if (arg[1] != '-') {
      opts->_token = arg + 1;

      switch (STARK_EXPECT_TRUE(match_shorthand(opts))) {
      case 0:
        break;
      case STARK_OPT_ASSIGN_FAILED:
        return false;
      case STARK_OPT_UNKNOWN:
        goto stark_opts_parse_unknown_option;
      }

      continue;
    }

    if (arg[2] == '\0') {
      nop = true;

      continue;
    }

    opts->_token = arg + 2;

    switch (STARK_EXPECT_TRUE(match_longhand(opts))) {
    case 0:
      break;
    case STARK_OPT_ASSIGN_FAILED:
      return false;
    case STARK_OPT_UNKNOWN:
      goto stark_opts_parse_unknown_option;
    }

    continue;

  stark_opts_parse_unknown_option:
    if (opts->err_callback) {
      opts->err_callback(STARK_OPTS_ERR_UNKNOWN_OPTION, arg);
    }

    return false;
  }

  for (int i = 0; i < opts->optc; i++) {
    struct stark_opt *const o = &opts->optv[i];

    if (o->mods & STARK_OPT_MOD_REQUIRED &&
        STARK_EXPECT_FALSE(!(o->mods & STARK_OPT_FOUND))) {
      return false; // required not found
    }
  }

  return true;
}
#endif // STARK_OPTS_IMPL

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // STARK_OPTS_H
