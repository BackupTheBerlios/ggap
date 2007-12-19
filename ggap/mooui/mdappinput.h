#ifndef MD_APP_INPUT_H
#define MD_APP_INPUT_H

#include <glib.h>

G_BEGIN_DECLS


typedef enum
{
    MD_APP_CMD_ZERO = 0,
    MD_APP_CMD_PYTHON_STRING,
    MD_APP_CMD_PYTHON_FILE,
    MD_APP_CMD_SCRIPT,
    MD_APP_CMD_OPEN_FILE,
    MD_APP_CMD_OPEN_URIS,
    MD_APP_CMD_QUIT,
    MD_APP_CMD_DIE,
    MD_APP_CMD_PRESENT,
    MD_APP_CMD_LAST
} MdAppCmdCode;


#if defined(WANT_MD_APP_CMD_STRINGS) || defined(WANT_MD_APP_CMD_CHARS)

/* 'g' is taken by ggap */
#define CMD_ZERO            "\0"
#define CMD_PYTHON_STRING   "p"
#define CMD_PYTHON_FILE     "P"
#define CMD_SCRIPT          "s"
#define CMD_OPEN_FILE       "f"
#define CMD_OPEN_URIS       "u"
#define CMD_QUIT            "q"
#define CMD_DIE             "d"
#define CMD_PRESENT         "r"

#endif

#ifdef WANT_MD_APP_CMD_CHARS

static const char *md_app_cmd_chars =
    CMD_ZERO
    CMD_PYTHON_STRING
    CMD_PYTHON_FILE
    CMD_SCRIPT
    CMD_OPEN_FILE
    CMD_OPEN_URIS
    CMD_QUIT
    CMD_DIE
    CMD_PRESENT
;

#endif /* WANT_MD_APP_CMD_CHARS */

#define MD_APP_INPUT_NAME_DEFAULT "main"

typedef struct _MdAppInput MdAppInput;


MdAppInput  *_md_app_input_new          (const char     *appname,
                                         const char     *name,
                                         gboolean        bind_default);
void         _md_app_input_free         (MdAppInput    *ch);

gboolean     _md_app_input_send_msg     (const char     *appname,
                                         const char     *name,
                                         const char     *data,
                                         gssize          len);
const char  *_md_app_input_get_path     (MdAppInput    *ch);


G_END_DECLS

#endif /* MD_APP_INPUT_H */
