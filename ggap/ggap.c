/* This file has been generated with opag 0.8.0.  */
/*
 *   ggap.c
 *
 *   Copyright (C) 2004-2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "config.h"
#include "gapapp.h"
#include "ggap-ui.h"
#include "mooutils/mooutils-fs.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/moostock.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int _ggap_parse_options (const char *const program_name,
                         const int         argc,
                         char **const      argv);

/********************************************************
 * command line parsing code generated by Opag
 * http://www.zero-based.org/software/opag/
 */
#ifndef STR_ERR_UNKNOWN_LONG_OPT
# define STR_ERR_UNKNOWN_LONG_OPT   "%s: unrecognized option `--%s'\n"
#endif

#ifndef STR_ERR_LONG_OPT_AMBIGUOUS
# define STR_ERR_LONG_OPT_AMBIGUOUS "%s: option `--%s' is ambiguous\n"
#endif

#ifndef STR_ERR_MISSING_ARG_LONG
# define STR_ERR_MISSING_ARG_LONG   "%s: option `--%s' requires an argument\n"
#endif

#ifndef STR_ERR_UNEXPEC_ARG_LONG
# define STR_ERR_UNEXPEC_ARG_LONG   "%s: option `--%s' doesn't allow an argument\n"
#endif

#ifndef STR_ERR_UNKNOWN_SHORT_OPT
# define STR_ERR_UNKNOWN_SHORT_OPT  "%s: unrecognized option `-%c'\n"
#endif

#ifndef STR_ERR_MISSING_ARG_SHORT
# define STR_ERR_MISSING_ARG_SHORT  "%s: option `-%c' requires an argument\n"
#endif

#define STR_HELP_GAP_CMD "\
  -g, --gap-cmd=COMMAND    GAP command line\n"

#define STR_HELP_EDITOR "\
  -e, --editor             Do not start GAP automatically\n"

#define STR_HELP_PURE_EDITOR "\
  -E, --pure-editor        Do not enable any GAP stuff\n"

#define STR_HELP_NEW_APP "\
  -n, --new-app            Run new instance of application\n"

#define STR_HELP_SIMPLE "\
      --simple             Simple mode\n"

#define STR_HELP_LOG "\
  -l, --log[=FILE]         Show debug output or write it to FILE\n"

#define STR_HELP_VERSION "\
      --version            Display version information and exit\n"

#define STR_HELP_HELP "\
  -h, --help               Display this help text and exit\n"

#define STR_HELP "\
  -g, --gap-cmd=COMMAND    GAP command line\n\
  -e, --editor             Do not start GAP automatically\n\
  -E, --pure-editor        Do not enable any GAP stuff\n\
  -n, --new-app            Run new instance of application\n\
      --simple             Simple mode\n\
  -l, --log[=FILE]         Show debug output or write it to FILE\n\
      --version            Display version information and exit\n\
  -h, --help               Display this help text and exit\n"

/* Set to 1 if option --gap-cmd (-g) has been specified.  */
char _ggap_opt_gap_cmd;

/* Set to 1 if option --editor (-e) has been specified.  */
char _ggap_opt_editor;

/* Set to 1 if option --pure-editor (-E) has been specified.  */
char _ggap_opt_pure_editor;

/* Set to 1 if option --new-app (-n) has been specified.  */
char _ggap_opt_new_app;

/* Set to 1 if option --simple has been specified.  */
char _ggap_opt_simple;

/* Set to 1 if option --log (-l) has been specified.  */
char _ggap_opt_log;

/* Set to 1 if option --version has been specified.  */
char _ggap_opt_version;

/* Set to 1 if option --help (-h) has been specified.  */
char _ggap_opt_help;

/* Argument to option --gap-cmd (-g).  */
const char *_ggap_arg_gap_cmd;

/* Argument to option --log (-l), or a null pointer if no argument.  */
const char *_ggap_arg_log;

/* Parse command line options.  Return index of first non-option argument,
   or -1 if an error is encountered.  */
int _ggap_parse_options (const char *const program_name, const int argc, char **const argv)
{
  static const char *const optstr__gap_cmd = "gap-cmd";
  static const char *const optstr__editor = "editor";
  static const char *const optstr__pure_editor = "pure-editor";
  static const char *const optstr__new_app = "new-app";
  static const char *const optstr__simple = "simple";
  static const char *const optstr__version = "version";
  static const char *const optstr__help = "help";
  int i = 0;
  _ggap_opt_gap_cmd = 0;
  _ggap_opt_editor = 0;
  _ggap_opt_pure_editor = 0;
  _ggap_opt_new_app = 0;
  _ggap_opt_simple = 0;
  _ggap_opt_log = 0;
  _ggap_opt_version = 0;
  _ggap_opt_help = 0;
  _ggap_arg_gap_cmd = 0;
  _ggap_arg_log = 0;
  while (++i < argc)
  {
    const char *option = argv [i];
    if (*option != '-')
      return i;
    else if (*++option == '\0')
      return i;
    else if (*option == '-')
    {
      const char *argument;
      size_t option_len;
      ++option;
      if ((argument = strchr (option, '=')) == option)
        goto error_unknown_long_opt;
      else if (argument == 0)
        option_len = strlen (option);
      else
        option_len = argument++ - option;
      switch (*option)
      {
       case '\0':
        return i + 1;
       case 'e':
        if (strncmp (option + 1, optstr__editor + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__editor;
            goto error_unexpec_arg_long;
          }
          _ggap_opt_editor = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 'g':
        if (strncmp (option + 1, optstr__gap_cmd + 1, option_len - 1) == 0)
        {
          if (argument != 0)
            _ggap_arg_gap_cmd = argument;
          else if (++i < argc)
            _ggap_arg_gap_cmd = argv [i];
          else
          {
            option = optstr__gap_cmd;
            goto error_missing_arg_long;
          }
          _ggap_opt_gap_cmd = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 'h':
        if (strncmp (option + 1, optstr__help + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__help;
            goto error_unexpec_arg_long;
          }
          _ggap_opt_help = 1;
          return i + 1;
        }
        goto error_unknown_long_opt;
       case 'l':
        if (strncmp (option + 1, "og", option_len - 1) == 0)
        {
          _ggap_arg_log = argument;
          _ggap_opt_log = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 'n':
        if (strncmp (option + 1, optstr__new_app + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__new_app;
            goto error_unexpec_arg_long;
          }
          _ggap_opt_new_app = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 'p':
        if (strncmp (option + 1, optstr__pure_editor + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__pure_editor;
            goto error_unexpec_arg_long;
          }
          _ggap_opt_pure_editor = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 's':
        if (strncmp (option + 1, optstr__simple + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__simple;
            goto error_unexpec_arg_long;
          }
          _ggap_opt_simple = 1;
          break;
        }
        goto error_unknown_long_opt;
       case 'v':
        if (strncmp (option + 1, optstr__version + 1, option_len - 1) == 0)
        {
          if (argument != 0)
          {
            option = optstr__version;
            goto error_unexpec_arg_long;
          }
          _ggap_opt_version = 1;
          return i + 1;
        }
       default:
       error_unknown_long_opt:
        fprintf (stderr, STR_ERR_UNKNOWN_LONG_OPT, program_name, option);
        return -1;
       error_missing_arg_long:
        fprintf (stderr, STR_ERR_MISSING_ARG_LONG, program_name, option);
        return -1;
       error_unexpec_arg_long:
        fprintf (stderr, STR_ERR_UNEXPEC_ARG_LONG, program_name, option);
        return -1;
      }
    }
    else
      do
      {
        switch (*option)
        {
         case 'E':
          _ggap_opt_pure_editor = 1;
          break;
         case 'e':
          _ggap_opt_editor = 1;
          break;
         case 'g':
          if (option [1] != '\0')
            _ggap_arg_gap_cmd = option + 1;
          else if (++i < argc)
            _ggap_arg_gap_cmd = argv [i];
          else
            goto error_missing_arg_short;
          option = "\0";
          _ggap_opt_gap_cmd = 1;
          break;
         case 'h':
          _ggap_opt_help = 1;
          return i + 1;
         case 'l':
          if (option [1] != '\0')
          {
            _ggap_arg_log = option + 1;
            option = "\0";
          }
          else
            _ggap_arg_log = 0;
          _ggap_opt_log = 1;
          break;
         case 'n':
          _ggap_opt_new_app = 1;
          break;
         default:
          fprintf (stderr, STR_ERR_UNKNOWN_SHORT_OPT, program_name, *option);
          return -1;
         error_missing_arg_short:
          fprintf (stderr, STR_ERR_MISSING_ARG_SHORT, program_name, *option);
          return -1;
        }
      } while (*++option != '\0');
  }
  return i;
}
/* end of generated code
 ********************************************************/


static void usage (void)
{
    g_print ("Usage: %s [OPTIONS] [FILES]\n", g_get_prgname ());
    g_print ("Options:\n");

    g_print ("%s", STR_HELP_GAP_CMD);
    g_print ("%s", STR_HELP_EDITOR);
    g_print ("%s", STR_HELP_PURE_EDITOR);
    g_print ("%s", STR_HELP_NEW_APP);
    g_print ("%s", STR_HELP_SIMPLE);
    g_print ("%s", STR_HELP_LOG);
    g_print ("%s", STR_HELP_VERSION);
    g_print ("%s", STR_HELP_HELP);
}

static void version (void)
{
    g_print ("ggap %s\n", GGAP_VERSION);
}


int main (int argc, char *argv[])
{
    MooApp *app;
    int opt_remain;
    char **files;

    gtk_init (&argc, &argv);

    opt_remain = _ggap_parse_options (g_get_prgname (), argc, argv);

    if (opt_remain < 0)
    {
        usage ();
        return 1;
    }

    if (_ggap_opt_help)
    {
        usage ();
        return 0;
    }
    else if (_ggap_opt_version)
    {
        version ();
        return 0;
    }

    if (_ggap_opt_log)
    {
        if (_ggap_arg_log)
            moo_set_log_func_file (_ggap_arg_log);
        else
            moo_set_log_func_window (TRUE);
    }

    GAP_APP_EDITOR_MODE = _ggap_opt_pure_editor != 0;

    files = moo_filenames_from_locale (argv + opt_remain);
    app = g_object_new (GAP_TYPE_APP,
                        "argv", argv,
                        "short-name", "ggap",
                        "full-name", "GGAP",
                        "description", "GGAP is a front end for GAP",
                        "run-output", (gboolean) !_ggap_opt_simple,
                        "run-input", (gboolean) !_ggap_opt_simple,
                        "default-ui", GGAP_UI,
                        "logo", MOO_STOCK_GGAP,

                        "gap-cmd-line", _ggap_arg_gap_cmd,
                        "editor-mode", (gboolean) _ggap_opt_editor,
                        "open-files", files && *files ? files : NULL,
                        "new-app", (gboolean) _ggap_opt_new_app,
                        "simple", (gboolean) _ggap_opt_simple,

                        NULL);
    g_free (files);

    moo_app_init (app);
    return moo_app_run (app);
}
