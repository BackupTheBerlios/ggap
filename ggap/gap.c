/*
 *   gap.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "config.h"
#include "gap.h"
#include "gapapp.h"
#include "ggap-restore.h"
#include "mooutils/mooutils-fs.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooprefs.h"
#include <string.h>
#include <errno.h>


char *
gap_escape_filename (const char *filename)
{
    g_return_val_if_fail (filename != NULL, NULL);
    return g_strescape (filename, NULL);
}


#define SAVE_WORKSPACE                                  \
"SaveWorkspace(\"%s\");\n"

#define SAVE_WORKSPACE_AND_GZIP                         \
SAVE_WORKSPACE                                          \
"if ARCH_IS_UNIX() then\n"                              \
"  Exec(\"rm -f\", Concatenation(\"%s\", \".gz\"));\n"  \
"  Exec(\"gzip\", \"%s\");\n"                           \
"fi;\n"


char *
ggap_pkg_exec_command (const char *cmdname,
                       const char *args)
{
    if (args && args[0])
        return g_strdup_printf ("$GGAP_EXEC_COMMAND(\"%s\", %s);\n", cmdname, args);
    else
        return g_strdup_printf ("$GGAP_EXEC_COMMAND(\"%s\");\n", cmdname);
}

char *
gap_cmd_save_workspace (const char *filename)
{
    char *escaped, *cmd;

    g_return_val_if_fail (filename != NULL, NULL);

    escaped = gap_escape_filename (filename);
    cmd = g_strdup_printf (SAVE_WORKSPACE, filename);

    g_free (escaped);
    return cmd;
}


static char *
save_workspace_init_file (const char *workspace)
{
    GString *contents;
    GError *error = NULL;
    char *filename;
    char *wsp_escaped;

    g_return_val_if_fail (workspace, NULL);

    filename = moo_tempnam ();
    g_return_val_if_fail (filename != NULL, NULL);

    contents = g_string_new (NULL);

    wsp_escaped = gap_escape_filename (workspace);
    g_string_append_printf (contents, SAVE_WORKSPACE_AND_GZIP,
                            wsp_escaped, wsp_escaped,
                            wsp_escaped);

    if (!_moo_save_file_utf8 (filename, contents->str, -1, &error))
    {
        g_critical ("%s: %s", G_STRLOC, error->message);
        g_error_free (error);
        g_free (filename);
        filename = NULL;
    }

    g_free (wsp_escaped);
    g_string_free (contents, TRUE);
    return filename;
}


static char *
gap_file_func_string (const char *filename,
                      const char *func)
{
    char *escaped, *string;

    g_return_val_if_fail (filename != NULL, NULL);
    g_return_val_if_fail (func != NULL, NULL);

    escaped = gap_escape_filename (filename);
    string = g_strdup_printf ("%s(\"%s\");\n", func, escaped);

    g_free (escaped);
    return string;
}


char*
gap_read_file_string (const char *filename)
{
    return gap_file_func_string (filename, "Read");
}


char*
gap_reread_file_string (const char *filename)
{
    return gap_file_func_string (filename, "Reread");
}


#ifdef __WIN32__
gboolean
gap_parse_cmd_line (const char *command_line,
                    char      **bin_dir_p,
                    char      **root_dir_p)
{
    char *bin_dir = NULL, *root_dir = NULL, *exe = NULL;

    g_return_val_if_fail (command_line && *command_line, FALSE);

    if (*command_line == '"')
    {
        char *second = strchr (command_line + 1, '"');

        if (!second || second == command_line + 1)
            return FALSE;

        exe = g_strndup (command_line, second - command_line - 1);
    }
    else
    {
        char **comps = g_strsplit_set (command_line, " \t", 2);

        if (!comps)
            return FALSE;

        exe = g_strdup (comps[0]);

        g_strfreev (comps);
    }

    if (!g_path_is_absolute (exe))
    {
        g_free (exe);
        return FALSE;
    }

    bin_dir = g_path_get_dirname (exe);
    root_dir = g_path_get_dirname (bin_dir);

    if (bin_dir_p)
        *bin_dir_p = bin_dir;
    else
        g_free (bin_dir);

    if (root_dir_p)
        *root_dir_p = root_dir;
    else
        g_free (root_dir);

    g_free (exe);

    return TRUE;
}
#endif


char *
gap_saved_workspace_filename (void)
{
    return moo_get_user_data_file ("workspace");
}


static char *
find_init_file (void)
{
    char **dirs;
    guint i, n_dirs = 0;

    dirs = moo_get_data_dirs (MOO_DATA_SHARE, &n_dirs);

    for (i = 0; i < n_dirs; ++i)
    {
        char *file = g_build_filename (dirs[i], "gap", "ggap-init.g", NULL);

        if (g_file_test (file, G_FILE_TEST_EXISTS))
        {
            g_strfreev (dirs);
            return file;
        }

        g_free (file);
    }

    g_strfreev (dirs);
    return NULL;
}

static char *
gen_temp_file (const char *contents)
{
    GError *error = NULL;
    char *filename;

    filename = moo_tempnam ();
    g_return_val_if_fail (filename != NULL, NULL);

    if (!_moo_save_file_utf8 (filename, contents, -1, &error))
    {
        g_critical ("%s: %s", G_STRLOC, error->message);
        g_error_free (error);
        g_free (filename);
        filename = NULL;
    }

    return filename;
}

static char *
gen_restore_file (const char *init_file)
{
    char *text = g_strdup_printf (GGAP_RESTORE, GGAP_API_VERSION, init_file, init_file);
    char *file = gen_temp_file (text);
    g_free (text);
    return file;
}

static GString *
make_command_line (const char *cmd_base,
                   const char *flags,
                   const char *custom_wsp,
                   gboolean    fancy)
{
    gboolean save_workspace;
    gboolean wsp_already_saved = FALSE;
    char *wsp_file = NULL;
    GString *cmd;

    save_workspace = moo_prefs_get_bool (GGAP_PREFS_GAP_SAVE_WORKSPACE);

    cmd = g_string_new (cmd_base);

    if (flags)
        g_string_append_printf (cmd, " %s", flags);

    if (!custom_wsp && save_workspace)
    {
        wsp_file = gap_saved_workspace_filename ();

        g_return_val_if_fail (wsp_file != NULL, cmd);

        wsp_already_saved = g_file_test (wsp_file, G_FILE_TEST_EXISTS);

        if (!wsp_already_saved)
        {
            char *gzipped = g_strdup_printf ("%s.gz", wsp_file);
            wsp_already_saved = g_file_test (gzipped, G_FILE_TEST_EXISTS);
            g_free (gzipped);
        }
    }

    if (custom_wsp)
    {
        g_string_append_printf (cmd, " -L \"%s\"", custom_wsp);
    }
    else if (save_workspace)
    {
        if (wsp_already_saved)
            g_string_append_printf (cmd, " -L \"%s\"", wsp_file);

        if (!wsp_already_saved && !moo_make_user_data_dir (NULL))
            g_critical ("%s: could not create user data dir", G_STRLOC);

        if (!wsp_already_saved)
        {
            char *file = save_workspace_init_file (wsp_file);
            g_string_append_printf (cmd, " \"%s\"", file);
            g_free (file);
        }
    }

    if (fancy)
    {
        char *init_file;

        if (!(init_file = find_init_file ()))
        {
            g_critical ("%s: could not find ggap-init.g file, broken installation?", G_STRLOC);
        }
        else
        {
            char *restore_file = gen_restore_file (init_file);
            if (!restore_file)
                g_critical ("%s: oops", G_STRLOC);
            else
                g_string_append_printf (cmd, " \"%s\"", restore_file);
            g_free (restore_file);
        }

        g_free (init_file);
    }

    g_free (wsp_file);
    return cmd;
}

char *
gap_make_cmd_line (const char *workspace,
                   const char *flags,
                   gboolean    fancy)
{
    const char *cmd_base;
    GString *cmd;

    cmd_base = moo_prefs_get_string (GGAP_PREFS_GAP_COMMAND);
    if (!cmd_base)
    {
        g_critical ("%s: gap command line not set", G_STRFUNC);
        cmd_base = "gap";
    }

#ifdef __WIN32__
    {
        static char *saved_bin_dir;
        static const char *saved_path;
        char *bin_dir;

        if (gap_parse_cmd_line (cmd_base, &bin_dir, NULL))
        {
            if (!saved_bin_dir || strcmp (saved_bin_dir, bin_dir))
            {
                char *path;

                if (!saved_path)
                    saved_path = g_getenv ("PATH");

                if (saved_path)
                    path = g_strdup_printf ("%s;%s", bin_dir, saved_path);
                else
                    path = g_strdup (bin_dir);

                g_setenv ("PATH", path, TRUE);
                g_free (path);

                saved_bin_dir = bin_dir;
                bin_dir = NULL;
            }

            g_free (bin_dir);
        }
        else
        {
            g_warning ("%s: could not parse command line `%s`",
                       G_STRLOC, cmd_base);
        }
    }
#endif

    cmd = make_command_line (cmd_base, flags, workspace, fancy);
    return g_string_free (cmd, FALSE);
}
