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

#include "stark/opts.h"

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
  void *val_ptr = NULL;

  if (opt->type == STARK_OPT_TYPE_SUBCOMMAND) {
    if (!run_subcommand(opts, opt)) {
      return false;
    }

    opts->_argc = 0;
    return true;
  }

  if ((opt->mods & STARK_OPT_MOD_ARRAY) && opt->arrc == opt->arrl) {
    fprintf(stderr, "array out of bounds\n");

    return false;
  }

  // This is how you get `-i3` and '-i 3' to work properly, same with
  // `--int=3` vs `--int 3`
  if (opt->mods & STARK_OPT_MOD_POSITIONAL) {
    str = *opts->_argv;
  } else if (opt->type == STARK_OPT_TYPE_BOOLEAN) {
    goto assign_opt_skip_arg;
  } else if (opts->_token != NULL && *opts->_token != '\0') {
    str = opts->_token;
    opts->_token = NULL;
  } else if (opts->_argc > 1) {
    opts->_argc--;
    opts->_argv++;
    str = *opts->_argv;
  } else {
    fprintf(stderr, "no value provided: '%s'\n",
            opts->_token != NULL ? opts->_token : *opts->_argv);

    return false;
  }

assign_opt_skip_arg:
  if (opt->assign != NULL) {
    if (!opt->assign(str, opt->dest, opt->arrc)) {
      return false;
    }

  } else if (opt->type == STARK_OPT_TYPE_BOOLEAN) {
    *(((bool *)opt->dest) + opt->arrc) = !*(((bool *)opt->dest) + opt->arrc);
    val_ptr = ((bool *)opt->dest) + opt->arrc;
  } else if (opt->type == STARK_OPT_TYPE_STRING) {
    *(((char const **)opt->dest) + opt->arrc) = str;
    val_ptr = ((char const **)opt->dest) + opt->arrc;
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
    }

    if (endptr == str) {
      fprintf(stderr, "not a number: '%s'\n", str);

      return false;
    } else if (errno == ERANGE) {
      fprintf(stderr, "out of range: '%s'\n", str);

      return false;
    }

    // The reason pointer math is used for assignment here is so that arrays
    // can be assigned properly. For normal non-array options it just works
    // normally (ptr + 0).
    switch (opt->type) {
    case STARK_OPT_TYPE_INTEGER:
      if (val.l > INT_MAX || val.l < INT_MIN) {
        fprintf(stderr, "integer out of range: '%s'\n", str);

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
    }
  }

  if (val_ptr != NULL && opt->handler.validate != NULL &&
      !opt->handler.validate(val_ptr, opt->ctx)) {
    return false;
  }

  if ((opt->mods & STARK_OPT_MOD_ARRAY)) {
    opt->arrc++;
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
  char const *const restrict token = opts->_token;
  char const *const restrict eq = STARK_STRCHR(token, '=');
  size_t const t_len = eq != NULL ? (size_t)(eq - token) : STARK_STRLEN(token);
  size_t i = hash_n(token, t_len) & (STARK_OPTS_LH_LUT_SIZE - 1);
  size_t probes = 0;

  while (true) {
    o = opts->_lh_lut[i];

    if (o == NULL) {
      return STARK_OPT_UNKNOWN;
    }

    if ((o->longhand != NULL && (STARK_EXPECT_TRUE(o->_long_len == t_len)) &&
         STARK_EXPECT_TRUE(STARK_MEMCMP(o->longhand, token, t_len) == 0)) ||
        (o->alias != NULL && STARK_EXPECT_TRUE(o->_alias_len == t_len) &&
         STARK_EXPECT_TRUE(STARK_MEMCMP(o->alias, token, t_len) == 0))) {
      break;
    }

    if (++probes == STARK_OPTS_LH_LUT_SIZE) {
      return STARK_OPT_UNKNOWN;
    }

    i = (i + 1) & (STARK_OPTS_LH_LUT_SIZE - 1);
  }

  opts->_token = eq != NULL ? eq + 1 : NULL;

  return assign_opt(opts, o) ? 0 : STARK_OPT_ASSIGN_FAILED;
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

    if (!assign_opt(opts, o)) {
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

STARK_COLD STARK_ALWAYS_INLINE static inline bool
require(const struct stark_opt *const restrict opt) {
  if (opt->longhand != NULL) {
    error("option '--%s' must have a destination pointer", opt->longhand);

    return false;
  } else if (opt->shorthand != '\0') {
    error("option '-%c' must have a destination pointer", opt->shorthand);

    return false;
  }

  return true;
}

STARK_COLD STARK_ALWAYS_INLINE STARK_FLATTEN static inline bool
lh_lut_push(struct stark_opts *const restrict opts,
            struct stark_opt *const restrict opt, bool const is_alias) {
  bool ok = true;
  char const *const restrict type = is_alias ? "alias" : "longhand";
  char const *const restrict opposite_type = is_alias ? "longhand" : "alias";
  char const *const restrict str = is_alias ? opt->alias : opt->longhand;
  size_t i = hash(str) & (STARK_OPTS_LH_LUT_SIZE - 1);
  size_t probes = 0;

  while (opts->_lh_lut[i] != NULL) {
    struct stark_opt const *prev = opts->_lh_lut[i];
    char const *prev_str = is_alias ? prev->alias : prev->longhand;
    char const *prev_opposite_str = is_alias ? prev->longhand : prev->alias;

    if (prev_str != NULL && STARK_STRCMP(prev_str, str) == 0) {
      error("duplicate %s '--%s'", type, str);
      ok = false;
    }

    if (prev_opposite_str != NULL &&
        STARK_STRCMP(prev_opposite_str, str) == 0) {
      error("%s '--%s' shadows %s '--%s'", type, str, opposite_type,
            prev_opposite_str);

      ok = false;
    }

    if (++probes == STARK_OPTS_LH_LUT_SIZE) {
      error(
          "longhand lookup table is full; define STARK_OPTS_LH_LUT_SIZE before "
          "inclusion with a greater limit or remove options");

      ok = false;
      break;
    }

    i = (i + 1) & (STARK_OPTS_LH_LUT_SIZE - 1);
  }

  opts->_lh_lut[i] = opt;
  return ok;
}

STARK_COLD STARK_ALWAYS_INLINE STARK_FLATTEN static inline bool
populate_longhand_lut(struct stark_opts *const restrict opts,
                      struct stark_opt *const restrict opt) {
  bool ok = true;

  // STARK_STRLEN() is used here to precalculate lengths so that it's not
  // called in the hot path. If the length is greater than 255, it's still used
  // as a fallback, but that is a very unlikely case.
  if (opt->longhand != NULL) {
    ok &= lh_lut_push(opts, opt, false);

    if ((opt->_long_len = (uint8_t)STARK_STRLEN(opt->longhand)) > 63) {
      error("longhand `--%s` exceeds max character limit of 63 characters",
            opt->longhand);

      ok = false;
    }
  }

  if (opt->alias != NULL) {
    ok &= lh_lut_push(opts, opt, true);

    if ((opt->_alias_len = (uint8_t)STARK_STRLEN(opt->alias)) > 63) {
      error("alias `--%s` exceeds max character limit of 63 characters",
            opt->alias);

      ok = false;
    }
  }

  return ok;
}

STARK_COLD STARK_ALWAYS_INLINE STARK_FLATTEN static inline bool
register_opt(struct stark_opts *const restrict opts,
             struct stark_opt *const restrict opt) {
  bool ok = true;

  if (opt->mods & STARK_OPT_MOD_POSITIONAL) {
    // Same reasoning here as for regular flags, precalculate the length to
    // minimize function calls in the hot path.
    if (opt->type == STARK_OPT_TYPE_SUBCOMMAND) {
      if (opt->usage == NULL) {
        error("positional subcommands must have a usage (name)");

        ok = false;
      } else {
        opt->_long_len = STARK_STRLEN(opt->usage);
      }
    } else if (opt->type == STARK_OPT_TYPE_BOOLEAN) {
      error("positional mod cannot be combined with boolean type");

      ok = false;
    }

    if (opts->_posc == STARK_OPTS_POS_LUT_SIZE) {
      error("positional argument count exceeds limit; define "
            "STARK_OPTS_POS_LUT_SIZE before inclusion with a greater limit or "
            "remove "
            "options");

      ok = false;
    } else {
      opts->_pos_lut[opts->_posc++] = opt;
    }

    return ok;
  } else if (opt->shorthand == '\0' && opt->longhand == NULL) {
    error("non-positional options must have either a shorthand or longhand");

    ok = false;
  }

  if (opt->shorthand == '\0') {
    goto register_opt_longhand;
  }

  if (opts->_sh_lut[opt->shorthand] == NULL) {
    opts->_sh_lut[opt->shorthand] = opt;
  } else {
    error("duplicate shorthand '-%c'", opt->shorthand);

    ok = false;
  }

register_opt_longhand:
  return ok &= populate_longhand_lut(opts, opt);
}

STARK_COLD STARK_ALWAYS_INLINE STARK_FLATTEN static inline bool
validate_opt(struct stark_opt *const restrict opt) {
  if (opt->assign != NULL) {
    if (opt->dest == NULL && !require(opt)) {
      return false;
    }

    return true;
  } else if (opt->handler.callback != NULL) {
    return true;
  }

  switch (opt->type) {
  case STARK_OPT_TYPE_STRING:
  case STARK_OPT_TYPE_DOUBLE:
  case STARK_OPT_TYPE_FLOAT:
  case STARK_OPT_TYPE_LONG:
  case STARK_OPT_TYPE_INTEGER:
  case STARK_OPT_TYPE_BOOLEAN:
    if (opt->dest == NULL && !require(opt)) {
      return false;
    }

    break;
  case STARK_OPT_TYPE_SUBCOMMAND:
    if (opt->ctx == NULL) {
      error("subcommand option missing context"); // placeholder
                                                  // developer-facing error

      return false;
    }

    break;
  case STARK_OPT_TYPE_HELP:
    break;
  default:
    error("invalid option type"); // placeholder developer-facing error

    return false;
  }

  return true;
}

STARK_COLD bool stark_opts_init(struct stark_opts *const restrict opts) {
  bool ok = true;

  if (opts == NULL || opts->optc == 0 || opts->optv == NULL ||
      !(ok = ((STARK_OPTS_LH_LUT_SIZE & (STARK_OPTS_LH_LUT_SIZE - 1)) == 0))) {
    STARK_TRAP();
  } else if (opts->_verified) {
    return true;
  }

  for (int i = 0; i < opts->optc; i++) {
    struct stark_opt *const restrict o = &opts->optv[i];

    // &= is used here to perserve the state of ok after each iteration. If ok
    // is false it will remain false regardless of what either function returns.
    ok &= validate_opt(o);
    ok &= register_opt(opts, o);
  }

  return opts->_verified = ok;
}

bool stark_opts_parse(struct stark_opts *const restrict opts, int const argc,
                      char const **const restrict argv) {
  if (!opts->_verified) {
    STARK_TRAP();
  }

  uint8_t pos_idx = 0;
  bool greedy_pos = false;

  // This is here to skip argv[0] which is usually the program name
  opts->_argc = argc - 1;
  opts->_argv = argv + 1;

  for (int i = 0; i < opts->optc; i++) {
    struct stark_opt *o = &opts->optv[i];

    o->mods &= ~STARK_OPT_FOUND;
    o->arrc = 0;
  }

  for (; opts->_argc > 0; opts->_argc--, opts->_argv++) {
    char const *arg = *opts->_argv;

    // This is here to skip any invalid arguments the OS may have put into argv
    if (arg == NULL || arg[0] == '\0') {
      continue;
    }

    // The reason greedy_pos is here is to skip flag checking for array
    // positionals or "greedy" ones
    if (greedy_pos || arg[0] != '-' || arg[1] == '\0') {
      if (opts->_posc == 0 || pos_idx >= opts->_posc) {
        goto stark_opts_parse_unknown_option;
      }

      struct stark_opt *const restrict o = opts->_pos_lut[pos_idx];

      if (o->type == STARK_OPT_TYPE_SUBCOMMAND) {
        size_t arg_len = STARK_STRLEN(arg);

        if (!(o->_long_len == arg_len &&
              STARK_EXPECT_TRUE(STARK_MEMCMP(arg, o->usage, o->_long_len) ==
                                0))) {
          goto stark_opts_parse_unknown_option;
        }
      }

      if (assign_opt(opts, o)) {
        // The reason this branch is here rather than just in the above if
        // statement is because otherwise a failed assignment wouldn't fall
        // through to the else block
        if (o->mods & STARK_OPT_MOD_ARRAY) {
          greedy_pos = true;
        }
      } else {
        return false;
      }

      if (!greedy_pos) {
        pos_idx++;
      }

      continue;
    }

    if (arg[1] != '-') {
      opts->_token = arg + 1;

      switch (match_shorthand(opts)) {
      case 0:
        break;
      case STARK_OPT_ASSIGN_FAILED:
        return false;
      case STARK_OPT_UNKNOWN:
        goto stark_opts_parse_unknown_option;
      }

      continue;
    }

    // This is here to stop parsing if a standalone `--` was encountered
    if (arg[2] == '\0') {
      opts->_argc--;
      opts->_argv++;

      break;
    }

    opts->_token = arg + 2;

    switch (match_longhand(opts)) {
    case 0:
      break;
    case STARK_OPT_ASSIGN_FAILED:
      return false;
    case STARK_OPT_UNKNOWN:
      goto stark_opts_parse_unknown_option;
    }

    continue;

  stark_opts_parse_unknown_option:
    fprintf(stderr, "unknown option: %s\n", arg);
    return false;
  }

  for (int i = 0; i < opts->optc; i++) {
    struct stark_opt *const o = &opts->optv[i];

    if (o->mods & STARK_OPT_MOD_REQUIRED && !(o->mods & STARK_OPT_FOUND)) {
      return false; // required not found
    }
  }

  return true;
}
