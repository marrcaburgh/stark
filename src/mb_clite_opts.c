#include "mb_clite_opts.h"

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void error(const char *errstr, ...) {
  va_list ap;

  va_start(ap, errstr);
  fprintf(stderr, "cli_opts error: ");
  vfprintf(stderr, errstr, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

static void cli_opts_usage(const struct mb_opt *const opt) {
  // TODO: print usage
}

static void cli_opts_help(struct mb_opts *app) {
  // TODO: print help
}

MB_HOT static inline bool cli_opt_assign(struct mb_opts *const app,
                                         const struct mb_opt *const opt) {
  switch (opt->type) {
  case MB_OPT_HELP:
    cli_opts_help(app);

    break;
  case MB_OPT_CALLBACK:
    opt->handler.cb(opt->ctx);

    break;
  case MB_OPT_BOOL:
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

    if (opt->assign != NULL && !opt->assign(str, opt->dest)) {
      return false;
    } else {
      char *endptr = NULL;
      union {
        long l;
        double d;
      } val;
      errno = 0;

      if (opt->type == MB_OPT_INT || opt->type == MB_OPT_LONG) {
        val.l = strtol(str, &endptr, 10);
      } else if (opt->type == MB_OPT_FLOAT || opt->type == MB_OPT_DBL) {
        val.d = strtod(str, &endptr);
      }

      if (endptr == str) {
        fprintf(stderr, "not a number: '%s'", str);

        return false;
      } else if (errno == ERANGE) {
        fprintf(stderr, "out of range: '%s'", str);

        return false;
      }

      switch (opt->type) {
      case MB_OPT_STR:
        *(const char **)opt->dest = str;

        break;
      case MB_OPT_INT:
        if (val.l > INT_MAX || val.l < INT_MIN) {
          fprintf(stderr, "integer out of range: '%s'", str);

          return false;
        }

        *(int *)opt->dest = (int)val.l;

        break;
      case MB_OPT_LONG:
        *(long *)opt->dest = val.l;

        break;
      case MB_OPT_FLOAT:
        *(float *)opt->dest = (float)val.d;

        break;
      case MB_OPT_DBL:
        *(double *)opt->dest = val.d;

        break;
      default:
        break;
      }
    }

    if (opt->handler.valid != NULL &&
        !opt->handler.valid(opt->dest, opt->ctx)) {
      return false;
    }
  }
  }

  return true;
}

static inline uint32_t hash(const char *str) {
  uint32_t h = 2166136261u;

  while (*str != '\0') {
    h ^= (unsigned char)*str++;
    h *= 16777619u;
  }

  return h;
}

static inline uint32_t hash_n(const char *str, size_t n) {
  uint32_t h = 2166136261u;

  for (size_t i = 0; i < n; i++) {
    h ^= (unsigned char)*str++;
    h *= 16777619u;
  }

  return h;
}

static int match_long(struct mb_opts *const app) {
  const char *eq = strchr(app->_token, '=');
  size_t t_len = eq != NULL ? (size_t)(eq - app->_token) : strlen(app->_token);
  size_t i = hash_n(app->_token, t_len) & (MB_LH_LUT_SIZE - 1);

  for (;;) {
    const struct mb_opt *o = app->lh_lut[i];

    if (o == NULL) {
      return MB_OPT_UNKNOWN;
    }

    if ((o->longhand != NULL && memcmp(o->longhand, app->_token, t_len) == 0 &&
         o->longhand[t_len] == '\0') ||
        (o->alias != NULL && memcmp(o->alias, app->_token, t_len) == 0 &&
         o->alias[t_len] == '\0')) {
      app->_token = eq != NULL ? (eq + 1) : NULL;
      return cli_opt_assign(app, o) ? 0 : 1;
    }

    i = (i + 1) & (MB_LH_LUT_SIZE - 1);
  }
}

static int match_short(struct mb_opts *const app) {
  while (app->_token != NULL && *app->_token) {
    const struct mb_opt *o = app->sh_lut[(unsigned char)*app->_token];

    if (o == NULL) {
      return MB_OPT_UNKNOWN;
    }

    app->_token = app->_token[1] != '\0' ? app->_token + 1 : NULL;

    if (!cli_opt_assign(app, o)) {
      return 1;
    }
  }

  return 0;
}

static bool require(const struct mb_opt *const opt, void *const ptr) {
  if (ptr) {
    return true;
  }

  const char *kind = opt->type == MB_OPT_CALLBACK ? "callback " : "";
  const char *target =
      opt->type == MB_OPT_CALLBACK ? "a function pointer" : "an outval";

  if (opt->longhand != NULL) {
    error("%soption '--%s' must have %s", kind, opt->longhand, target);
  } else {
    error("%soption '-%c' must have %s", kind, opt->shorthand, target);
  }

  printf("require");

  return false;
}

MB_COLD bool _mb_opts_init(struct mb_opts *const app) {
  bool ok = true;

  memset(app->sh_lut, 0, sizeof(app->sh_lut));
  memset(app->lh_lut, 0, sizeof(app->lh_lut));

  for (struct mb_opt *o = (struct mb_opt *)app->opts; o->type != MB_OPT_END;
       o++) {
    switch (o->type) {
    case MB_OPT_CUSTOM:
    case MB_OPT_STR:
    case MB_OPT_DBL:
    case MB_OPT_FLOAT:
    case MB_OPT_LONG:
    case MB_OPT_INT:
    case MB_OPT_BOOL:
      if (!require(o, o->dest)) {
        ok = false;
      }

      break;
    case MB_OPT_CALLBACK:
      if (o->assign) {
        error("handlers cannot be paired with actions");
      }

      if (!require(o, o->handler.cb)) {
        ok = false;
      }

      break;
    case MB_OPT_END:
    case MB_OPT_HELP:
      break;
    default:
      error("invalid mb_opt '%d'", o->type);
      ok = false;
    }

    if (o->shorthand == '\0' && o->longhand == NULL) {
      error("options must have either a shorthand or longhand");
      ok = false;
    }

    if (o->shorthand != '\0') {
      if (app->sh_lut[o->shorthand] == NULL) {
        app->sh_lut[o->shorthand] = o;
      } else {
        error("duplicate shorthand '-%c'", o->shorthand);
      }
    }

    if (o->longhand != NULL) {
      uint32_t hsh = hash(o->longhand);
      size_t i = hsh & (MB_LH_LUT_SIZE - 1);

      while (app->lh_lut[i] != NULL) {
        const struct mb_opt *prev = app->lh_lut[i];

        if (prev->longhand != NULL) {
          if (strcmp(prev->longhand, o->longhand) == 0) {
            error("duplicate longhand '--%s'", o->longhand);

            ok = false;
            goto lh_dup;
          }
        }

        if (prev->alias != NULL) {
          if (strcmp(prev->alias, o->longhand) == 0) {
            error("longhand '--%s' shadows alias '--%s'", o->longhand,
                  prev->alias);

            ok = false;
            goto lh_dup;
          }
        }

        i = (i + 1) & (MB_LH_LUT_SIZE - 1);
      }

      app->lh_lut[i] = o;
    }

  lh_dup:

    if (o->alias != NULL) {
      uint32_t hsh = hash(o->alias);
      size_t i = hsh & (MB_LH_LUT_SIZE - 1);

      while (app->lh_lut[i] != NULL) {
        const struct mb_opt *prev = app->lh_lut[i];

        if (prev->alias != NULL) {
          if (strcmp(prev->alias, o->alias) == 0) {
            error("duplicate alias '--%s'", o->alias);

            ok = false;
            goto al_dup;
          }
        }

        if (prev->longhand != NULL) {
          if (strcmp(prev->longhand, o->alias) == 0) {
            error("alias '--%s' shadows longhand '--%s'", o->alias,
                  prev->longhand);

            ok = false;
            goto al_dup;
          }
        }

        i = (i + 1) & (MB_LH_LUT_SIZE - 1);
      }
      app->lh_lut[i] = o;
    }

  al_dup:
    continue;
  }

  return app->verified = ok;
}

bool mb_opts_parse(struct mb_opts *const app, const int argc,
                   const char **const argv) {
  if (!app->verified) {
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

      switch (match_short(app)) {
      case 0:
        break;
      case 1:
        return false;
      case MB_OPT_UNKNOWN:
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

    switch (match_long(app)) {
    case 0:
      break;
    case 1:
      return false;
    case MB_OPT_UNKNOWN:
      goto unknown;
    }

    continue;

  unknown:
    fprintf(stderr, "unknown option: %s\n", arg);
    return false;
  }

  return true;
}
