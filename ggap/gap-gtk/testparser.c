#include <config.h>
#include "gapparser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_READLINE_READLINE_H
#include <readline/readline.h>
#include <readline/history.h>
#endif


G_GNUC_NORETURN static void
usage (const char *prg)
{
    g_print ("usage: %s <file>\n"
             "       %s -c <script>\n"
             "       %s\n", prg, prg, prg);
    exit (1);
}


static int
run_interactive (void)
{
#ifndef HAVE_READLINE_READLINE_H
    return 0;
#else
    using_history ();

    while (TRUE)
    {
        char *line;
        GapParseResult parse_result;

        line = readline ("gap> ");

        if (!line)
        {
            g_print ("\n");
            return 0;
        }

        parse_result = gap_parse (line);

        if (parse_result == GAP_PARSE_INCOMPLETE)
        {
            GString *input = g_string_new (line);
            free (line);

            while (TRUE)
            {
                line = readline ("> ");

                if (!line)
                {
                    g_print ("\n");
                    return 0;
                }

                g_string_append (input, "\n");
                g_string_append (input, line);
                free (line);

                parse_result = gap_parse (input->str);

                if (parse_result != GAP_PARSE_INCOMPLETE)
                {
                    line = g_string_free (input, FALSE);
                    break;
                }
            }
        }

        add_history (line);
        free (line);
    }
#endif
}


int main (int argc, char *argv[])
{
    char **p;
    GError *error = NULL;

    if (argc < 2)
        return run_interactive ();

    if (!strcmp (argv[1], "-c"))
    {
        char *code = g_strjoinv (" ", argv + 2);
        gap_parse (code);
        g_free (code);
        exit (0);
    }

    for (p = argv + 1; *p; ++p)
    {
        char *code;

        if (!g_file_get_contents (*p, &code, NULL, &error))
        {
            g_print ("%s\n", error->message);
            exit (1);
        }

        if (gap_parse (code) != GAP_PARSE_OK)
        {
            g_print ("file: %s\n", *p);
            exit (1);
        }

        g_free (code);
    }

    return 0;
}
