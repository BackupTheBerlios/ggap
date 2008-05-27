
/* Generated data (by glib-mkenums) */

#include "ggap-enums.h"

#include "gapparser.h"

GType
gap_parse_result_get_type (void)
{
    static GType etype;
    if (G_UNLIKELY (!etype))
    {
        static const GEnumValue values[] = {
            { GAP_PARSE_OK, (char*) "GAP_PARSE_OK", (char*) "ok" },
            { GAP_PARSE_ERROR, (char*) "GAP_PARSE_ERROR", (char*) "error" },
            { GAP_PARSE_INCOMPLETE, (char*) "GAP_PARSE_INCOMPLETE", (char*) "incomplete" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static ("GapParseResult", values);
    }
    return etype;
}

#include "gapprocess.h"

GType
gap_state_get_type (void)
{
    static GType etype;
    if (G_UNLIKELY (!etype))
    {
        static const GEnumValue values[] = {
            { GAP_DEAD, (char*) "GAP_DEAD", (char*) "dead" },
            { GAP_IN_PROMPT, (char*) "GAP_IN_PROMPT", (char*) "in-prompt" },
            { GAP_LOADING, (char*) "GAP_LOADING", (char*) "loading" },
            { GAP_BUSY, (char*) "GAP_BUSY", (char*) "busy" },
            { GAP_BUSY_INTERNAL, (char*) "GAP_BUSY_INTERNAL", (char*) "busy-internal" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static ("GapState", values);
    }
    return etype;
}

#include "gapworksheet.h"

GType
gap_file_type_get_type (void)
{
    static GType etype;
    if (G_UNLIKELY (!etype))
    {
        static const GEnumValue values[] = {
            { GAP_FILE_WORKSHEET, (char*) "GAP_FILE_WORKSHEET", (char*) "worksheet" },
            { GAP_FILE_TEXT, (char*) "GAP_FILE_TEXT", (char*) "text" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static ("GapFileType", values);
    }
    return etype;
}


/* Generated data ends here */

