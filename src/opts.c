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

static void usage(const struct mbx_opt *const restrict opt) {
  // TODO: print usage
}

static void help(struct mbx_opts *const restrict opts) {
  // TODO: print help
}

MB_HOT __attribute__((always_inline)) static inline bool
assign_opt(struct mbx_opts *const restrict opts,
           const struct mbx_opt *const restrict opt) {
  uint16_t base_type = opt->type & MBX_OPT_TYPE_MASK;

  switch (base_type) {
  case MBX_OPT_TYPE_HELP:
    help(opts);

    break;
  case MBX_OPT_TYPE_CALLBACK:
    opt->handler.callback(opt->ctx);

    break;
  case MBX_OPT_TYPE_BOOL:
    *(bool *)opt->dest = !*(bool *)opt->dest;

    break;
  default: {
    const char *str;

    if (opt->type & MBX_OPT_MOD_POSITIONAL) {
      str = *opts->_argv;
      goto pos_skip;
    }

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

  pos_skip:
    if (opt->assign != NULL) {
      if (!opt->assign(str, opt->dest)) {
        return false;
      }
    } else if (base_type == MBX_OPT_TYPE_STR) {
      *(const char **)opt->dest = str;
    } else {
      char *endptr = NULL;
      union {
        long l;
        double d;
      } val;
      errno = 0;

      if (base_type == MBX_OPT_TYPE_INT || base_type == MBX_OPT_TYPE_LONG) {
        val.l = strtol(str, &endptr, 10);
      } else if (base_type == MBX_OPT_TYPE_FLOAT ||
                 base_type == MBX_OPT_TYPE_DBL) {
        val.d = strtod(str, &endptr);
      }

      if (endptr == str) {
        fprintf(stderr, "not a number: '%s'\n", str);

        return false;
      } else if (errno == ERANGE) {
        fprintf(stderr, "out of range: '%s'\n", str);

        return false;
      }

      switch (base_type) {
      case MBX_OPT_TYPE_INT:
        if (val.l > INT_MAX || val.l < INT_MIN) {
          fprintf(stderr, "integer out of range: '%s'\n", str);

          return false;
        }

        *(int *)opt->dest = (int)val.l;

        break;
      case MBX_OPT_TYPE_LONG:
        *(long *)opt->dest = val.l;

        break;
      case MBX_OPT_TYPE_FLOAT:
        *(float *)opt->dest = (float)val.d;

        break;
      case MBX_OPT_TYPE_DBL:
        *(double *)opt->dest = val.d;
      default:
        break;
      }
    }

    if (opt->handler.validate != NULL &&
        !opt->handler.validate(opt->dest, opt->ctx)) {
      return false;
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

static inline uint32_t hash_n(const char *restrict str, const size_t n) {
  uint32_t h = 2166136261u;

  for (size_t i = 0; i < n; i++) {
    h ^= (unsigned char)*str++;
    h *= 16777619u;
  }

  return h;
}

MB_HOT __attribute__((always_inline)) static inline int
match_longhand(struct mbx_opts *const restrict opts) {
  const struct mbx_opt *restrict o;
  const char *const restrict token = opts->_token;
  const char *const restrict eq = strchr(token, '=');
  const size_t t_len = eq != NULL ? (size_t)(eq - token) : strlen(token);
  size_t i = hash_n(token, t_len) & (MB_LH_LUT_SIZE - 1);

  while (true) {
    o = opts->_lh_lut[i];

    if (o == NULL) {
      return MBX_OPT_UNKNOWN;
    }

    if (o->longhand == NULL) {
      goto alias;
    }

    const uint8_t long_len = o->lens >> 4;

    if (((long_len != 0xF && long_len == t_len) ||
         strlen(o->longhand) == t_len) &&
        likely(memcmp(o->longhand, token, t_len) == 0)) {
      break;
    }

  alias:
    if (o->alias == NULL) {
      goto next;
    }

    const uint8_t alias_len = o->lens & 0x0F;

    if (((alias_len != 0xF && alias_len == t_len) ||
         strlen(o->alias) == t_len) &&
        likely(memcmp(o->alias, token, t_len) == 0)) {
      break;
    }

  next:
    i = (i + 1) & (MB_LH_LUT_SIZE - 1);
  }

  opts->_token = eq != NULL ? (eq + 1) : NULL;
  return assign_opt(opts, o) ? 0 : 1;
}

MB_HOT __attribute__((always_inline)) static inline int
match_shorthand(struct mbx_opts *const restrict opts) {
  const struct mbx_opt *restrict o;
  const bool combined = opts->_token[1] != '\0';

  while (opts->_token != NULL) {
    o = opts->_sh_lut[(unsigned char)*opts->_token];

    if (unlikely(o == NULL)) {
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

MB_COLD static void error(const char *const errstr, ...) {
  va_list ap;

  va_start(ap, errstr);
  fprintf(stderr, "mbx_opts error: ");
  vfprintf(stderr, errstr, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

MB_COLD static bool require(const struct mbx_opt *const restrict opt) {
  uint32_t base_type = opt->type & MBX_OPT_TYPE_MASK;
  const char *kind = base_type == MBX_OPT_TYPE_CALLBACK ? "callback " : "";
  const char *target =
      base_type == MBX_OPT_TYPE_CALLBACK ? "a function pointer" : "an outval";

  if (opt->longhand != NULL) {
    error("%soption '--%s' must have %s", kind, opt->longhand, target);

    return false;
  } else if (opt->shorthand != '\0') {
    error("%soption '-%c' must have %s", kind, opt->shorthand, target);

    return false;
  }

  return true;
}

MB_COLD static bool lh_lut_push(struct mbx_opts *const restrict opts,
                                struct mbx_opt *const restrict opt,
                                const bool is_alias) {
  bool ok = true;
  const char *const restrict kind = is_alias ? "alias" : "longhand";
  const char *const restrict other_kind = is_alias ? "longhand" : "alias";
  const char *const restrict str = is_alias ? opt->alias : opt->longhand;
  size_t i = hash(str) & (MB_LH_LUT_SIZE - 1);

  while (opts->_lh_lut[i] != NULL) {
    const struct mbx_opt *prev = opts->_lh_lut[i];
    const char *prev_same = is_alias ? prev->alias : prev->longhand;
    const char *prev_other = is_alias ? prev->longhand : prev->alias;

    if (prev_same != NULL && strcmp(prev_same, str) == 0) {
      error("duplicate %s '--%s'", kind, str);
      ok = false;
    }

    if (prev_other != NULL && strcmp(prev_other, str) == 0) {
      error("%s '--%s' shadows %s '--%s'", kind, str, other_kind, prev_other);
      ok = false;
    }

    i = (i + 1) & (MB_LH_LUT_SIZE - 1);
  }

  opts->_lh_lut[i] = opt;
  return ok;
}

MB_COLD static bool populate_longhand_lut(struct mbx_opts *const restrict opts,
                                          struct mbx_opt *const restrict opt) {
  bool ok = true;
  uint8_t long_len = 0;
  uint8_t alias_len = 0;

  if (opt->longhand != NULL) {
    ok &= lh_lut_push(opts, opt, false);
    long_len = strlen(opt->longhand);
  }

  if (opt->alias != NULL) {
    ok &= lh_lut_push(opts, opt, true);
    alias_len = strlen(opt->alias);
  }

  opt->lens = ((long_len > 15 ? 0xF : long_len) << 4) |
              ((alias_len > 15 ? 0xF : alias_len));

  return ok;
}

MB_COLD bool mbx_opts_init(struct mbx_opts *const restrict opts, const int optc,
                           struct mbx_opt *const restrict optv) {
  if (opts == NULL) {
    error("mbx_opts_init: opts cannot be null");

    return false;
  } else if (optc == 0) {
    error("mbx_opts_init: optc cannot be zero");

    return false;
  } else if (optv == NULL) {
    error("mbx_opts_init: optv cannot be null");

    return false;
  }

  bool ok = true;
  uint8_t posc = 0;

  memset(opts->_sh_lut, 0, sizeof(opts->_sh_lut));
  memset(opts->_lh_lut, 0, sizeof(opts->_lh_lut));

  for (int i = 0; i < optc; i++) {
    struct mbx_opt *const o = &optv[i];
    const enum mbx_opt_type base_type = o->type & MBX_OPT_TYPE_MASK;
    bool pos = false;

    switch (base_type) {
    case MBX_OPT_TYPE_CUSTOM:
    case MBX_OPT_TYPE_STR:
    case MBX_OPT_TYPE_DBL:
    case MBX_OPT_TYPE_FLOAT:
    case MBX_OPT_TYPE_LONG:
    case MBX_OPT_TYPE_INT:
    case MBX_OPT_TYPE_BOOL:
      if (o->dest == NULL && !require(o)) {
        ok = false;
      }

      if (o->type & MBX_OPT_MOD_POSITIONAL) {
        pos = true;
      }

      break;
    case MBX_OPT_TYPE_CALLBACK:
      if (o->assign != NULL) {
        error("assigners cannot be paired with actions");

        ok = false;
      }

      if (o->handler.callback == NULL && !require(o)) {
        ok = false;
      }

      break;
    case MBX_OPT_TYPE_SUBCOMMAND:
      pos = true;
      break;
    case MBX_OPT_TYPE_HELP:
      break;
    default:
      error("invalid type for option '%d'", o->type);

      ok = false;
    }

    if (pos) {
      goto positional;
    } else if (o->shorthand == '\0' && o->longhand == NULL) {
      error("non-positonal options must have either a shorthand or longhand");

      ok = false;
    }

    if (o->shorthand == '\0') {
      goto longhand;
    }

    if (opts->_sh_lut[o->shorthand] == NULL) {
      opts->_sh_lut[o->shorthand] = o;
    } else {
      error("duplicate shorthand '-%c'", o->shorthand);

      ok = false;
    }

  longhand:
    ok &= populate_longhand_lut(opts, o);
    continue;

  positional:
    o->lens = strlen(o->longhand);
    opts->_pos_lut[posc++] = o;
  }

  opts->posc = posc;

  return opts->_verified = ok;
}

MB_COLD bool mbx_opts_parse(struct mbx_opts *const restrict opts,
                            const int argc, const char **const restrict argv) {
  if (!opts->_verified) {
    error("not verified, did you forget 'mbx_opts_init'?");

    return false;
  }

  opts->_argc = argc - 1;
  opts->_argv = argv + 1;

  uint8_t pos_idx = 0;

  for (; opts->_argc != 0; opts->_argc--, opts->_argv++) {
    const char *arg = *opts->_argv;

    if (arg == NULL || arg[0] == '\0') {
      continue;
    }

    if (arg[0] != '-' || arg[1] == '\0') {
      const mbx_opt *const restrict o = opts->_pos_lut[pos_idx];

      if (o == NULL) {
        goto non_pos;
      }

      size_t arg_len = strlen(arg);

      if ((o->type & MBX_OPT_TYPE_MASK) == MBX_OPT_TYPE_SUBCOMMAND &&
          o->lens == arg_len &&
          memcmp(arg, opts->_pos_lut[pos_idx]->longhand, o->lens) == 0) {
        return mbx_opts_parse((struct mbx_opts *const restrict)o->ctx,
                              opts->_argc, opts->_argv);
      } else if (o->type & MBX_OPT_MOD_POSITIONAL && !assign_opt(opts, o)) {
        return false;
      } else {
        pos_idx++;
      }

    non_pos:
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

  return true;
}
