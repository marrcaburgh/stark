#include "cli_opts.h"

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void cli_opts_error(const char *errstr, ...) {
  va_list ap;

  va_start(ap, errstr);
  fprintf(stderr, "cli_opts error: ");
  vfprintf(stderr, errstr, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

static bool cli_opt_require(const struct cli_opt *const opt, void *const ptr) {
  if (ptr) {
    return true;
  }

  const char *kind = opt->type == CLI_OPT_TYPE_ACTION ? "callback " : "";
  const char *target =
      opt->type == CLI_OPT_TYPE_ACTION ? "a function pointer" : "an outval";

  if (opt->longhand != NULL) {
    cli_opts_error("%soption '--%s' must have %s", kind, opt->longhand, target);
  } else {
    cli_opts_error("%soption '-%c' must have %s", kind, opt->shorthand, target);
  }

  return false;
}

static bool cli_opts_verify(const struct cli_opt *const opts) {
  bool ok = true;

  for (const struct cli_opt *o = opts; o->type != CLI_OPT_TYPE_END; o++) {
    switch (o->type) {
    case CLI_OPT_TYPE_END:
    case CLI_OPT_TYPE_HELP:
    case CLI_OPT_TYPE_ACTION:
    case CLI_OPT_TYPE_STRING:
    case CLI_OPT_TYPE_DOUBLE:
    case CLI_OPT_TYPE_FLOAT:
    case CLI_OPT_TYPE_LONG:
    case CLI_OPT_TYPE_INTEGER:
    case CLI_OPT_TYPE_BOOLEAN:
      break;
    default:
      cli_opts_error("invalid cli_opt_type '%d'", o->type);
      ok = false;
    }

    if (o->shorthand == '\0' && o->longhand == NULL) {
      cli_opts_error("options must have either a shorthand or longhand");
      ok = false;
    }

    switch (o->type) {
    case CLI_OPT_TYPE_ACTION:
      if (!cli_opt_require(o, o->action)) {
        ok = false;
      }
      break;
    case CLI_OPT_TYPE_HELP:
      break;
    default:
      if (!cli_opt_require(o, o->dest)) {
        ok = false;
      }
    }

    for (const struct cli_opt *next = o + 1; next->type != CLI_OPT_TYPE_END;
         next++) {
      if (o->shorthand && o->shorthand == next->shorthand) {
        cli_opts_error("duplicate shorthand '-%c'", o->shorthand);
        ok = false;
      }

      if (o->longhand && o->longhand == next->longhand) {
        cli_opts_error("duplicate longhand '--%s'", o->longhand);
        ok = false;
      }
    }
  }

  return ok;
}

void cli_opts_init(struct cli_opts *const app, struct cli_opt *const opts,
                   const char *const desc) {
  if (app == NULL) {
    cli_opts_error("app is NULL");
    exit(EXIT_FAILURE);
  }

  if (opts == NULL) {
    cli_opts_error("opts is NULL");
    exit(EXIT_FAILURE);
  }

  if (!cli_opts_verify(opts)) {
    exit(EXIT_FAILURE);
  }

  memset(app, 0, sizeof(*app));

  app->opts = opts;
  app->desc = desc;
}

static void cli_opts_usage(const struct cli_opt *const opt) {
  // TODO: print usage
}

static void cli_opts_help(struct cli_opts *app) {
  // TODO: print help
}

static bool cli_opt_assign(struct cli_opts *const app,
                           const struct cli_opt *const opt) {
  switch (opt->type) {
  case CLI_OPT_TYPE_HELP:
    cli_opts_help(app);
    break;
  case CLI_OPT_TYPE_ACTION:
    opt->action(opt->ctx);
    break;
  case CLI_OPT_TYPE_BOOLEAN:
    *(bool *)opt->dest = !*(bool *)opt->dest;
    break;
  default: {
    const char *str;

    if (app->token && *app->token != '\0') {
      str = app->token;
      app->token = NULL;
    } else if (app->argc > 1) {
      app->argc--;
      app->argv++;
      str = *app->argv;
    } else {
      cli_opts_error("no value '%s'", *app->argv);
      return false;
    }

    char *endptr;
    union {
      long l;
      double d;
    } val;

    if (opt->type == CLI_OPT_TYPE_INTEGER || opt->type == CLI_OPT_TYPE_LONG) {
      val.l = strtol(str, &endptr, 10);
    } else if (opt->type == CLI_OPT_TYPE_FLOAT ||
               opt->type == CLI_OPT_TYPE_DOUBLE) {
      val.d = strtod(str, &endptr);
    }

    if (endptr == str) {
      cli_opts_error("value not a number '%s'", str);
      return false;
    } else if (errno == ERANGE) {
      cli_opts_error("value out of range '%s'", str);
      return false;
    }

    switch (opt->type) {
    case CLI_OPT_TYPE_STRING:
      *(const char **)opt->dest = str;
      break;
    case CLI_OPT_TYPE_INTEGER:
      if (val.l > INT_MAX || val.l < INT_MIN) {
        cli_opts_error("integer value out of range '%s'", str);
        return false;
      }
      *(int *)opt->dest = (int)val.l;
      break;
    case CLI_OPT_TYPE_LONG:
      *(long *)opt->dest = val.l;
      break;
    case CLI_OPT_TYPE_FLOAT:
      *(float *)opt->dest = (float)val.d;
      break;
    case CLI_OPT_TYPE_DOUBLE:
      *(double *)opt->dest = val.d;
      break;
    default:
      return false;
    }
  }
  }

  return true;
}

static bool cli_opts_match_long(struct cli_opts *const app) {
  const char *eq = strchr(app->token, '=');

  size_t key_len = eq != NULL ? (size_t)(eq - app->token) : strlen(app->token);

  for (const struct cli_opt *o = app->opts; o->type != CLI_OPT_TYPE_END; o++) {
    if (o->longhand != NULL && strlen(o->longhand) == key_len &&
        strncmp(o->longhand, app->token, key_len) == 0) {
      app->token = eq != NULL ? (eq + 1) : NULL;

      return cli_opt_assign(app, o);
    }
  }

  return false;
}

static int cli_opts_match_short(struct cli_opts *const app) {
  while (app->token != NULL && *app->token != '\0') {
    for (const struct cli_opt *o = app->opts; o->type != CLI_OPT_TYPE_END;
         o++) {
      if (o->shorthand == *app->token) {
        app->token = (app->token[1] != '\0') ? (app->token + 1) : NULL;

        if (!cli_opt_assign(app, o)) {
          return false;
        }

        if (app->token == NULL) {
          return true;
        }

        break;
      }
    }
  }

  return false;
}

void cli_opts_parse(struct cli_opts *const app, const int argc,
                    const char **const argv) {
  app->argc = argc - 1;
  app->argv = argv + 1;

  for (; app->argc; app->argc--, app->argv++) {
    const char *arg = app->argv[0];

    if (arg == NULL || arg[0] == '\0') {
      continue;
    }

    if (arg[0] != '-' || arg[1] == '\0') {
      continue;
    }

    // shorthand option
    if (arg[1] != '-') {
      app->token = arg + 1;

      if (!cli_opts_match_short(app)) {
        goto unknown;
      }

      continue;
    }

    // longhand option
    if (arg[2] == '\0') {
      app->argc--;
      app->argc++;
      break;
    }

    app->token = arg + 2;

    if (!cli_opts_match_long(app)) {
      goto unknown;
    }

    continue;

  unknown:
    printf("unknown option: %s\n", arg);
  }
}
