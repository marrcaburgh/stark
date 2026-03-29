//
// libmbx - a C99+ utility library - mbx_opts - blazing-fast feature-full
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

#include "mbx/opts.h"

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MBX_OPT_UNKNOWN 2
#define MBX_OPT_ASSIGN_FAILED 1

MBX_COLD MBX_ALWAYS_INLINE static void
usage(const struct mbx_opt *const restrict opt) {
  // TODO: print usage
}

MBX_COLD MBX_ALWAYS_INLINE static void
help(struct mbx_opts *const restrict opts) {
  // TODO: print help
}

MBX_HOT MBX_NOINLINE static bool
run_subcommand(struct mbx_opts *const restrict opts,
               struct mbx_opt *const restrict opt) {
  return mbx_opts_parse(opt->ctx, opts->_argc, opts->_argv);
}

MBX_HOT MBX_ALWAYS_INLINE static inline bool
assign_opt(struct mbx_opts *const restrict opts,
           struct mbx_opt *const restrict opt) {
  opt->mods |= MBX_OPT_FOUND;

  if (opt->handler.callback != NULL) {
    opt->handler.callback(opt->ctx);

    return true;
  }

  switch (opt->type) {
  case MBX_OPT_TYPE_HELP:
    help(opts);

    break;
  default: {
    char const *str;
    void *val_ptr = NULL;

    if (opt->type == MBX_OPT_TYPE_SUBCOMMAND) {
      if (!run_subcommand(opts, opt)) {
        return false;
      }

      opts->_argc = 0;
      return true;
    }

    if ((opt->mods & MBX_OPT_MOD_ARRAY) && opt->arrc == opt->arrl) {
      fprintf(stderr, "array out of bounds");

      return false;
    }

    if (opt->mods & MBX_OPT_MOD_POSITIONAL) {
      str = *opts->_argv;
      goto skip_next;
    } else if (opt->type == MBX_OPT_TYPE_BOOL) {
      goto skip_next;
    }

    // This is how you get `-i3` and '-i 3' to work properly, same with
    // `--int=3` vs `--int 3`
    if (opts->_token != NULL && *opts->_token != '\0') {
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

  skip_next:
    if (opt->assign != NULL) {
      if (!opt->assign(str, opt->dest, opt->arrc)) {
        return false;
      }

    } else if (opt->type == MBX_OPT_TYPE_BOOL) {
      *(((bool *)opt->dest) + opt->arrc) = !*(((bool *)opt->dest) + opt->arrc);
      val_ptr = ((bool *)opt->dest) + opt->arrc;
    } else if (opt->type == MBX_OPT_TYPE_STR) {
      *(((char const **)opt->dest) + opt->arrc) = str;
      val_ptr = ((char const **)opt->dest) + opt->arrc;
    } else {
      char *endptr = NULL;
      union {
        long l;
        double d;
      } val;
      errno = 0;

      if (opt->type == MBX_OPT_TYPE_INT || opt->type == MBX_OPT_TYPE_LONG) {
        val.l = strtol(str, &endptr, 10);
      } else if (opt->type == MBX_OPT_TYPE_FLOAT ||
                 opt->type == MBX_OPT_TYPE_DBL) {
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
      case MBX_OPT_TYPE_INT:
        if (val.l > INT_MAX || val.l < INT_MIN) {
          fprintf(stderr, "integer out of range: '%s'\n", str);

          return false;
        }

        *(((int *)opt->dest) + opt->arrc) = (int)val.l;
        val_ptr = ((int *)opt->dest) + opt->arrc;

        break;
      case MBX_OPT_TYPE_LONG:
        *(((long *)opt->dest) + opt->arrc) = val.l;
        val_ptr = ((long *)opt->dest) + opt->arrc;

        break;
      case MBX_OPT_TYPE_FLOAT:
        *(((float *)opt->dest) + opt->arrc) = (float)val.d;
        val_ptr = ((float *)opt->dest) + opt->arrc;

        break;
      case MBX_OPT_TYPE_DBL:
        *(((double *)opt->dest) + opt->arrc) = val.d;
        val_ptr = ((double *)opt->dest) + opt->arrc;

        break;
      }
    }

    if (val_ptr != NULL && opt->handler.validate != NULL &&
        !opt->handler.validate(val_ptr, opt->ctx)) {
      return false;
    }

    if ((opt->mods & MBX_OPT_MOD_ARRAY)) {
      opt->arrc++;
    }
  }
  }

  return true;
}

static inline uint32_t hash(const char *restrict str) {
  uint32_t h = 2166136261u;

  while (*str != '\0') {
    h ^= (unsigned char)*str++;
    h *= 16777619u;
  }

  return h;
}

static inline uint32_t hash_n(const char *restrict str, size_t const n) {
  uint32_t h = 2166136261u;

  for (size_t i = 0; i < n; i++) {
    h ^= (unsigned char)*str++;
    h *= 16777619u;
  }

  return h;
}

MBX_HOT MBX_ALWAYS_INLINE MBX_FLATTEN static inline int
match_longhand(struct mbx_opts *const restrict opts) {
  struct mbx_opt *restrict o;
  char const *const restrict token = opts->_token;
  char const *const restrict eq = strchr(token, '=');
  size_t const t_len = eq != NULL ? (size_t)(eq - token) : mbx_strlen(token);
  size_t i = hash_n(token, t_len) & (MBX_OPTS_LH_LUT_SIZE - 1);
  size_t probes = 0;

  while (true) {
    o = opts->_lh_lut[i];

    if (o == NULL) {
      return MBX_OPT_UNKNOWN;
    }

    if ((o->longhand != NULL && MBX_EXPECT_TRUE(o->long_len == t_len) &&
         MBX_EXPECT_TRUE(mbx_bcmp(o->longhand, token, t_len))) ||
        (o->alias != NULL && MBX_EXPECT_TRUE(o->alias_len == t_len) &&
         MBX_EXPECT_TRUE(mbx_bcmp(o->alias, token, t_len)))) {
      break;
    }

    if (++probes == MBX_OPTS_LH_LUT_SIZE) {
      return MBX_OPT_UNKNOWN;
    }

    i = (i + 1) & (MBX_OPTS_LH_LUT_SIZE - 1);
  }

  opts->_token = eq != NULL ? eq + 1 : NULL;
  return assign_opt(opts, o) ? 0 : MBX_OPT_ASSIGN_FAILED;
}

MBX_HOT MBX_ALWAYS_INLINE MBX_FLATTEN static inline int
match_shorthand(struct mbx_opts *const restrict opts) {
  struct mbx_opt *restrict o;
  bool const combined = opts->_token[1] != '\0';

  while (opts->_token != NULL) {
    o = opts->_sh_lut[(unsigned char)*opts->_token];

    if (MBX_EXPECT_FALSE(o == NULL)) {
      return MBX_OPT_UNKNOWN;
    }

    opts->_token = opts->_token[1] != '\0' ? &opts->_token[1] : NULL;

    if (!assign_opt(opts, o)) {
      return MBX_OPT_ASSIGN_FAILED;
    }

    if (!combined) {
      break;
    }
  }

  return 0;
}

MBX_COLD MBX_ALWAYS_INLINE static void error(const char *const errstr, ...) {
  va_list ap;

  va_start(ap, errstr);
  fprintf(stderr, "mbx_opts error: ");
  vfprintf(stderr, errstr, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

MBX_COLD MBX_ALWAYS_INLINE static bool
require(const struct mbx_opt *const restrict opt) {
  if (opt->longhand != NULL) {
    error("option '--%s' must have a destination pointer", opt->longhand);

    return false;
  } else if (opt->shorthand != '\0') {
    error("option '-%c' must have a destination pointer", opt->shorthand);

    return false;
  }

  return true;
}

MBX_COLD MBX_ALWAYS_INLINE static bool
lh_lut_push(struct mbx_opts *const restrict opts,
            struct mbx_opt *const restrict opt, bool const is_alias) {
  bool ok = true;
  char const *const restrict type = is_alias ? "alias" : "longhand";
  char const *const restrict other_type = is_alias ? "longhand" : "alias";
  char const *const restrict str = is_alias ? opt->alias : opt->longhand;
  size_t i = hash(str) & (MBX_OPTS_LH_LUT_SIZE - 1);
  size_t probes = 0;

  while (opts->_lh_lut[i] != NULL) {
    struct mbx_opt const *prev = opts->_lh_lut[i];
    char const *prev_type = is_alias ? prev->alias : prev->longhand;
    char const *prev_other_type = is_alias ? prev->longhand : prev->alias;

    if (prev_type != NULL && strcmp(prev_type, str) == 0) {
      error("duplicate %s '--%s'", type, str);
      ok = false;
    }

    if (prev_other_type != NULL && strcmp(prev_other_type, str) == 0) {
      error("%s '--%s' shadows %s '--%s'", type, str, other_type,
            prev_other_type);

      ok = false;
    }

    if (++probes == MBX_OPTS_LH_LUT_SIZE) {
      error("longhand lookup table is full; define MBX_OPTS_LH_LUT_SIZE before "
            "inclusion with a greater limit or remove options");

      ok = false;
      break;
    }

    i = (i + 1) & (MBX_OPTS_LH_LUT_SIZE - 1);
  }

  opts->_lh_lut[i] = opt;
  return ok;
}

MBX_COLD MBX_ALWAYS_INLINE static bool
populate_longhand_lut(struct mbx_opts *const restrict opts,
                      struct mbx_opt *const restrict opt) {
  bool ok = true;

  // mbx_strlen() is used here to precalculate lengths so that mbx_strlen is not
  // called in the hot path. if the length is greater than 255, its still used
  // as a fallback, but that is a very unlikely case.
  if (opt->longhand != NULL) {
    ok &= lh_lut_push(opts, opt, false);

    if ((opt->long_len = (uint8_t)mbx_strlen(opt->longhand)) > 63) {
      error("longhand `--%s` exceeds max character limit of 63 characters",
            opt->longhand);

      ok = false;
    }
  }

  if (opt->alias != NULL) {
    ok &= lh_lut_push(opts, opt, true);

    if ((opt->alias_len = (uint8_t)mbx_strlen(opt->alias)) > 63) {
      error("alias `--%s` exceeds max character limit of 63 characters",
            opt->alias);

      ok = false;
    }
  }

  return ok;
}

MBX_COLD MBX_ALWAYS_INLINE bool
register_opt(struct mbx_opts *const restrict opts,
             struct mbx_opt *const restrict opt) {
  bool ok = true;

  if (opt->mods & MBX_OPT_MOD_POSITIONAL) {
    // Same reasoning here as for regular flags, precalculate the length to
    // minimize function calls in the hot path.
    if (opt->type == MBX_OPT_TYPE_SUBCOMMAND) {
      if (opt->usage == NULL) {
        error("positional subcommands must have a usage (name)");

        ok = false;
      } else {
        opt->long_len = mbx_strlen(opt->usage);
      }
    } else if (opt->type == MBX_OPT_TYPE_BOOL) {
      error("positional mod cannot be combined with boolean type");

      ok = false;
    }

    if (opts->_posc == MBX_OPTS_POS_LUT_SIZE) {
      error("positional argument count exceeds limit; define "
            "MBX_OPTS_POS_LUT_SIZE before inclusion with a greater limit or "
            "remove "
            "options");

      ok = false;
    } else {
      opts->_pos_lut[opts->_posc++] = opt;
    }

    return ok;
  } else if (opt->shorthand == '\0' && opt->longhand == NULL) {
    error("non-positonal options must have either a shorthand or longhand");

    ok = false;
  }

  if (opt->shorthand == '\0') {
    goto longhand;
  }

  if (opts->_sh_lut[opt->shorthand] == NULL) {
    opts->_sh_lut[opt->shorthand] = opt;
  } else {
    error("duplicate shorthand '-%c'", opt->shorthand);

    ok = false;
  }

longhand:
  return ok &= populate_longhand_lut(opts, opt);
}

MBX_COLD bool validate_opt(struct mbx_opt *const restrict opt) {
  if (opt->assign != NULL) {
    if (opt->dest == NULL && !require(opt)) {
      return false;
    }

    return true;
  } else if (opt->handler.callback != NULL) {
    return true;
  }

  switch (opt->type) {
  case MBX_OPT_TYPE_STR:
  case MBX_OPT_TYPE_DBL:
  case MBX_OPT_TYPE_FLOAT:
  case MBX_OPT_TYPE_LONG:
  case MBX_OPT_TYPE_INT:
  case MBX_OPT_TYPE_BOOL:
    if (opt->dest == NULL && !require(opt)) {
      return false;
    }

    break;
  case MBX_OPT_TYPE_SUBCOMMAND:
    if (opt->ctx == NULL) {
      error("subcommands must have a context");

      return false;
    }

    break;
  case MBX_OPT_TYPE_HELP:
    break;
  default:
    error("invalid type for option '%d'", opt->type);

    return false;
  }

  return true;
}

MBX_COLD bool mbx_opts_init(struct mbx_opts *const restrict opts) {
  bool ok = true;

  if (opts == NULL) {
    error("mbx_opts_init: opts cannot be null");

    return false;
  } else if (opts->optc == 0) {
    error("mbx_opts_init: optc cannot be zero");

    return false;
  } else if (opts->optv == NULL) {
    error("mbx_opts_init: optv cannot be null");

    return false;
  } else if (!(ok = (MBX_OPTS_LH_LUT_SIZE & (MBX_OPTS_LH_LUT_SIZE - 1)) == 0)) {
    error("mbx_opts_init: MBX_OPTS_LH_LUT_SIZE must be a power of two");
  } else if (opts->_verified) {
    error("mbx_opts_init: already verified");

    return true;
  }

  for (int i = 0; i < opts->optc; i++) {
    struct mbx_opt *const restrict o = &opts->optv[i];

    // &= is used here to set ok only if either function returned false,
    // otherwise the state wouldnt be perserved across options
    ok &= validate_opt(o);
    ok &= register_opt(opts, o);
  }

  return opts->_verified = ok;
}

bool mbx_opts_parse(struct mbx_opts *const restrict opts, int const argc,
                    char const **const restrict argv) {
  if (!opts->_verified) {
    error("not verified, did you forget 'mbx_opts_init'?");

    return false;
  }

  uint8_t pos_idx = 0;
  bool greedy_pos = false;

  opts->_argc = argc - 1;
  opts->_argv = argv + 1;

  for (; opts->_argc > 0; opts->_argc--, opts->_argv++) {
    char const *arg = *opts->_argv;

    // This is here to skip any invalid arguments the OS may have put into argv.
    if (arg == NULL || arg[0] == '\0') {
      continue;
    }

    // The reason greedy_pos is here is to skip flag checking for array
    // positionals or "greedy" ones
    if (greedy_pos || arg[0] != '-' || arg[1] == '\0') {
      if (opts->_posc == 0 || pos_idx >= opts->_posc) {
        goto unknown;
      }

      struct mbx_opt *const restrict o = opts->_pos_lut[pos_idx];

      if (o->type == MBX_OPT_TYPE_SUBCOMMAND) {
        size_t arg_len = mbx_strlen(arg);

        if (!(o->long_len == arg_len && mbx_bcmp(arg, o->usage, o->long_len))) {
          goto unknown;
        }
      }

      if (assign_opt(opts, o)) {
        // The reason this branch is here rather than just in the above if
        // statement is because otherwise a failed assignment wouldn't fall
        // through to the else block
        if (o->mods & MBX_OPT_MOD_ARRAY) {
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
      case MBX_OPT_ASSIGN_FAILED:
        return false;
      case MBX_OPT_UNKNOWN:
        goto unknown;
      }

      continue;
    }

    if (arg[2] == '\0') {
      opts->_argc--;
      opts->_argv++;

      break;
    }

    opts->_token = arg + 2;

    switch (match_longhand(opts)) {
    case 0:
      break;
    case MBX_OPT_ASSIGN_FAILED:
      return false;
    case MBX_OPT_UNKNOWN:
      goto unknown;
    }

    continue;

  unknown:
    fprintf(stderr, "unknown option: %s\n", arg);
    return false;
  }

  for (int i = 0; i < opts->optc; i++) {
    struct mbx_opt *const o = &opts->optv[i];

    o->arrc = 0;

    if (o->mods & MBX_OPT_MOD_REQUIRED && !(o->mods & MBX_OPT_FOUND)) {
      return false; // required not found
    }

    o->mods &= ~MBX_OPT_FOUND;
  }

  return true;
}
