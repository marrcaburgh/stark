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

static void error(const char *const errstr, ...) {
  va_list ap;

  va_start(ap, errstr);
  fprintf(stderr, "mb_opts error: ");
  vfprintf(stderr, errstr, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

static void usage(const struct mbx_opt *const restrict opt) {
  // TODO: print usage
}

static void help(struct mbx_opts *const restrict app) {
  // TODO: print help
}

MB_HOT static inline bool assign_opt(struct mbx_opts *const restrict app,
                                     const struct mbx_opt *const restrict opt) {
  switch (opt->type) {
  case MBX_OPT_TYPE_HELP:
    help(app);

    break;
  case MBX_OPT_TYPE_CALLBACK:
    opt->handler.callback(opt->ctx);

    break;
  case MBX_OPT_TYPE_BOOL:
    *(bool *)opt->dest = !*(bool *)opt->dest;

    break;
  default: {
    const char *str;

    if (app->_token != NULL && *app->_token != '\0') {
      str = app->_token;
      app->_token = NULL;
    } else if (app->_argc > 1) {
      app->_argc--;
      app->_argv++;
      str = *app->_argv;
    } else {
      fprintf(stderr, "no value provided: '%s'\n",
              app->_token != NULL ? app->_token : *app->_argv);

      return false;
    }

    if (opt->assign != NULL) {
      if (!opt->assign(str, opt->dest)) {
        return false;
      }
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

      switch (opt->type) {
      case MBX_OPT_TYPE_STR:
        *(const char **)opt->dest = str;

        break;
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

        break;
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

MB_HOT static inline int match_longhand(struct mbx_opts *const restrict app) {
  const struct mbx_opt *o;
  const char *eq = strchr(app->_token, '=');
  const size_t t_len =
      eq != NULL ? (size_t)(eq - app->_token) : strlen(app->_token);
  size_t i = hash_n(app->_token, t_len) & (MB_LH_LUT_SIZE - 1);

  while (true) {
    o = app->_lh_lut[i];

    if (o == NULL) {
      return MBX_OPT_UNKNOWN;
    }

    if (o->longhand == NULL) {
      goto alias;
    }

    const uint8_t long_len = o->lens >> 4;

    if (((long_len != 0xF && likely(long_len == t_len)) ||
         likely(strlen(o->longhand) == t_len)) &&
        likely(memcmp(o->longhand, app->_token, t_len) == 0)) {
      break;
    }

  alias:
    if (o->alias == NULL) {
      goto next;
    }

    const uint8_t alias_len = o->lens & 0x0F;

    if (((alias_len != 0xF && likely(alias_len == t_len)) ||
         likely(strlen(o->alias) == t_len)) &&
        likely(memcmp(o->alias, app->_token, t_len) == 0)) {
      break;
    }

  next:
    i = (i + 1) & (MB_LH_LUT_SIZE - 1);
    continue;
  }

  app->_token = eq != NULL ? (eq + 1) : NULL;
  return assign_opt(app, o) ? 0 : 1;
}

MB_HOT static inline int match_shorthand(struct mbx_opts *const restrict app,
                                         const bool combined) {
  const struct mbx_opt *o;
ms_loop:
  o = app->_sh_lut[(unsigned char)*app->_token];

  if (o == NULL) {
    return MBX_OPT_UNKNOWN;
  }

  app->_token = app->_token[1] != '\0' ? app->_token + 1 : NULL;

  if (!assign_opt(app, o)) {
    return MBX_OPT_ASSIGN_FAILED;
  }

  if (combined && app->_token != NULL) {
    goto ms_loop;
  }

  return 0;
}

static bool require(const struct mbx_opt *const restrict opt,
                    void *const restrict ptr) {
  if (ptr != NULL) {
    return true;
  }

  const char *kind = opt->type == MBX_OPT_TYPE_CALLBACK ? "callback " : "";
  const char *target =
      opt->type == MBX_OPT_TYPE_CALLBACK ? "a function pointer" : "an outval";

  if (opt->longhand != NULL) {
    error("%soption '--%s' must have %s", kind, opt->longhand, target);
  } else if (opt->shorthand != '\0') {
    error("%soption '-%c' must have %s", kind, opt->shorthand, target);
  }

  return false;
}

MB_COLD bool populate_longhand_lut(struct mbx_opts *const restrict app,
                                   struct mbx_opt *const restrict opt) {
  bool ok = true;
  uint8_t long_len = 0;
  uint8_t alias_len = 0;

  if (opt->longhand != NULL) {
    uint32_t hsh = hash(opt->longhand);
    size_t i = hsh & (MB_LH_LUT_SIZE - 1);

    while (app->_lh_lut[i] != NULL) {
      const struct mbx_opt *prev = app->_lh_lut[i];

      if (prev->longhand != NULL &&
          strcmp(prev->longhand, opt->longhand) == 0) {
        error("duplicate longhand '--%s'", opt->longhand);

        ok = false;
        goto lh_dup;
      }

      if (prev->alias != NULL && strcmp(prev->alias, opt->longhand) == 0) {
        error("longhand '--%s' shadows alias '--%s'", opt->longhand,
              prev->alias);

        ok = false;
        goto lh_dup;
      }

      i = (i + 1) & (MB_LH_LUT_SIZE - 1);
    }

    app->_lh_lut[i] = opt;
    long_len = strlen(opt->longhand);
  }

lh_dup:

  if (opt->alias != NULL) {
    uint32_t hsh = hash(opt->alias);
    size_t i = hsh & (MB_LH_LUT_SIZE - 1);

    while (app->_lh_lut[i] != NULL) {
      const struct mbx_opt *prev = app->_lh_lut[i];

      if (prev->alias != NULL && strcmp(prev->alias, opt->alias) == 0) {
        error("duplicate alias '--%s'", opt->alias);

        ok = false;
        goto al_dup;
      }

      if (prev->longhand != NULL && strcmp(prev->longhand, opt->alias) == 0) {
        error("alias '--%s' shadows longhand '--%s'", opt->alias,
              prev->longhand);

        ok = false;
        goto al_dup;
      }

      i = (i + 1) & (MB_LH_LUT_SIZE - 1);
    }

    app->_lh_lut[i] = opt;
    alias_len = strlen(opt->alias);
  }

  opt->lens = ((long_len > 15 ? 0xF : long_len) << 4) |
              ((alias_len > 15 ? 0xF : alias_len));

al_dup:
  return ok;
}

MB_COLD bool _mbx_opts_init(struct mbx_opts *const restrict opts,
                            struct mbx_opt *const opt, const size_t optc) {
  if (opts == NULL) {
    error("opts cannot be null");

    return false;
  }

  if (opt == NULL) {
    error("opt cannot be null");

    return false;
  }

  if (optc == 0) {
    error("optc cannot be zero");

    return false;
  }

  bool ok = true;

  memset(opts->_sh_lut, 0, sizeof(opts->_sh_lut));
  memset(opts->_lh_lut, 0, sizeof(opts->_lh_lut));

  for (size_t i = 0; i < optc; i++) {
    struct mbx_opt *const o = &opt[i];

    switch (o->type) {
    case MBX_OPT_TYPE_CUSTOM:
    case MBX_OPT_TYPE_STR:
    case MBX_OPT_TYPE_DBL:
    case MBX_OPT_TYPE_FLOAT:
    case MBX_OPT_TYPE_LONG:
    case MBX_OPT_TYPE_INT:
    case MBX_OPT_TYPE_BOOL:
      if (!require(o, o->dest)) {
        ok = false;
      }

      break;
    case MBX_OPT_TYPE_CALLBACK:
      if (o->assign) {
        error("assigners cannot be paired with actions");

        ok = false;
      }

      if (!require(o, o->handler.callback)) {
        ok = false;
      }

      break;
    case MBX_OPT_TYPE_HELP:
      break;
    default:
      error("invalid type for option '%d'", o->type);

      ok = false;
    }

    if (o->shorthand == '\0' && o->longhand == NULL) {
      error("options must have either a shorthand or longhand");

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
  }

  return opts->_verified = ok;
}

bool mbx_opts_parse(struct mbx_opts *const restrict app, const int argc,
                    const char **const argv) {
  if (!app->_verified) {
    error("not verified, did you forget 'mb_opts_init'?");

    return false;
  }

  app->_argc = argc - 1;
  app->_argv = argv + 1;

  for (; app->_argc != 0; app->_argc--, app->_argv++) {
    const char *arg = app->_argv[0];

    if (arg == NULL || arg[0] == '\0') {
      continue;
    }

    if (arg[0] != '-' || arg[1] == '\0') {
      continue;
    }

    if (arg[1] != '-') {
      app->_token = arg + 1;

      switch (match_shorthand(app, app->_token[1] != '\0')) {
      case 0:
        break;
      case 1:
        return false;
      case MBX_OPT_UNKNOWN:
        goto unknown;
      }

      continue;
    }

    if (arg[2] == '\0') {
      app->_argc--;
      app->_argv++;
      break;
    }

    app->_token = arg + 2;

    switch (match_longhand(app)) {
    case 0:
      break;
    case 1:
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
