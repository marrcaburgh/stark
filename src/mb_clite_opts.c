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

static inline bool cli_opt_assign(struct mb_opts *const app,
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

static int match_long(struct mb_opts *const app) {
  const char *eq = strchr(app->_token, '=');
  size_t t_len = eq != NULL ? (size_t)(eq - app->_token) : strlen(app->_token);

  for (const struct mb_opt *o = app->opts; o->type != MB_OPT_END; o++) {
    if ((o->lens & 0x0F) == t_len && o->longhand != NULL) {
      if (memcmp(o->longhand, app->_token, t_len) == 0) {
        goto found;
      }
    }

    if ((o->lens >> 4) == t_len && o->alias != NULL) {
      if (memcmp(o->alias, app->_token, t_len) == 0) {
        goto found;
      }
    }

    continue;

  found:
    app->_token = eq != NULL ? (eq + 1) : NULL;
    return cli_opt_assign(app, o) ? 0 : 1;
  }

  return 2;
}

static int match_short(struct mb_opts *const app) {
  const struct mb_opt *o;

  while (app->_token != NULL && *app->_token != '\0') {
    for (o = app->opts; o->shorthand != *app->_token; o++) {
      if (o->type == MB_OPT_END) {
        return 2;
      }
    }

    app->_token = app->_token[1] != '\0' ? app->_token + 1 : NULL;

    if (!cli_opt_assign(app, o)) {
      return 1;
    }

    if (app->_token == NULL) {
      break;
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

  return false;
}

MB_COLD bool _mb_opts_init(struct mb_opts *const app) {
  bool ok = true;

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

    for (const struct mb_opt *next = o + 1; next->type != MB_OPT_END; next++) {
      if (o->shorthand != '\0' && o->shorthand == next->shorthand) {
        error("duplicate shorthand '-%c'", o->shorthand);
        ok = false;
      }

      if (o->longhand != NULL && next->longhand != NULL &&
          strcmp(o->longhand, next->longhand) == 0) {
        error("duplicate longhand '--%s'", o->longhand);
        ok = false;
      }

      if (o->alias != NULL && next->alias != NULL &&
          strcmp(o->alias, next->alias) == 0) {
        error("duplicate alias '--%s'", o->alias);
        ok = false;
      }
    }

    uint8_t long_len = 0;
    uint8_t alias_len = 0;

    if (o->longhand != NULL) {
      const size_t len = strlen(o->longhand);
      long_len = (len > 15) ? 15 : (uint8_t)len;
    }

    if (o->alias != NULL) {
      const size_t len = strlen(o->alias);
      alias_len = (len > 15) ? 15 : (uint8_t)len;
    }

    o->lens = alias_len << 4 | long_len;
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
      case 2:
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
    case 2:
      goto unknown;
    }

    continue;

  unknown:
    fprintf(stderr, "unknown option: %s\n", arg);
    return false;
  }

  return true;
}
