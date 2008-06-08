/*
 *   mdfileops.c
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
#include "mooui/mdfileops.h"
#include "mooui/mdasyncop.h"
#include "mooui/mddocument.h"
#include "mooui/marshals.h"
#include "mooui/mddialogs.h"
#include "mooutils/mootype-macros.h"
#include "mooutils/mooutils-misc.h"
/* sys/stat.h macros */
#include "mooutils/mooutils-fs.h"
#include "mooutils/moospawn.h"
#include "mooutils/mooi18n.h"
#include <glib/gstdio.h>
#include <glib/gmappedfile.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if GLIB_CHECK_VERSION(2,16,0)
#define HAVE_GIO
#include <gio/gio.h>
#endif


#define MD_CHECK_ARG(condition,error,retval)                                \
G_STMT_START {                                                              \
    g_return_val_if_fail (!error || !*error, retval);                       \
                                                                            \
    if (!(condition))                                                       \
    {                                                                       \
        g_critical ("%s: condition " #condition " failed", G_STRFUNC);      \
        g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_INVAL,             \
                     "Internal error: condition " #condition " failed");    \
        return retval;                                                      \
    }                                                                       \
} G_STMT_END

int
md_open_file_for_reading (const char  *filename,
                          GError     **error)
{
    int fd;
    struct stat statbuf;

    MD_CHECK_ARG (filename != NULL, error, -1);

    errno = 0;

    if (g_stat (filename, &statbuf) != 0)
    {
        int err = errno;
        g_set_error (error, G_FILE_ERROR,
                     g_file_error_from_errno (err),
                     "%s", g_strerror (err));
        return -1;
    }

    if (
#ifdef S_ISREG
        !S_ISREG (statbuf.st_mode) ||
#endif
        0)
    {
        g_set_error (error, G_FILE_ERROR,
                     G_FILE_ERROR_FAILED,
                     "Not a regular file.");
        return -1;
    }

    errno = 0;
    fd = g_open (filename, O_BINARY | O_RDONLY, 0);

    if (fd == -1)
    {
        int err = errno;
        g_set_error (error, G_FILE_ERROR,
                     g_file_error_from_errno (err),
                     "%s", g_strerror (err));
        return -1;
    }

    return fd;
}


int
md_open_file_for_writing (const char  *filename,
                          GError     **error)
{
    int fd;
    mode_t mode;

    MD_CHECK_ARG (filename != NULL, error, -1);

#ifdef __WIN32__
    mode = S_IRUSR | S_IWUSR;
#else
    /* XXX umask? */
//     if (doc->priv->mode_set)
//         mode = (edit->priv->mode & (S_IRWXU | S_IRWXG | S_IRWXO)) | S_IRUSR | S_IWUSR;
//     else
        mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
#endif

    errno = 0;
    fd = g_open (filename, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, mode);

    if (fd == -1)
    {
        int err = errno;
        g_set_error (error, G_FILE_ERROR,
                     g_file_error_from_errno (err),
                     "%s", g_strerror (err));
    }

    return fd;
}


GQuark
md_file_error_quark (void)
{
    static GQuark q;

    if (G_UNLIKELY (!q))
        q = g_quark_from_static_string ("md-file-error");

    return q;
}


/*************************************************************************/
/* MdMountOperation
 */

#ifdef HAVE_GIO

#define MD_TYPE_MOUNT_OPERATION     (md_mount_operation_get_type ())
#define MD_MOUNT_OPERATION(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_MOUNT_OPERATION, MdMountOperation))
#define MD_IS_MOUNT_OPERATION(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_MOUNT_OPERATION))

typedef struct MdMountOperation MdMountOperation;
typedef struct MdMountOperationClass MdMountOperationClass;

static void     md_mount_operation_dispose      (GObject            *object);
static void     md_mount_operation_ask_password (GMountOperation    *op,
                                                 const char         *message,
                                                 const char         *default_user,
                                                 const char         *default_domain,
                                                 GAskPasswordFlags flags);
static void     md_mount_operation_ask_question (GMountOperation    *op,
                                                 const char         *message,
                                                 const char         *choices[]);

struct MdMountOperation {
    GMountOperation base;
    GtkWidget *parent;
};

struct MdMountOperationClass {
    GMountOperationClass base_class;
};

MOO_DEFINE_TYPE_STATIC (MdMountOperation, md_mount_operation, G_TYPE_MOUNT_OPERATION)

static void
md_mount_operation_init (MdMountOperation *op)
{
    op->parent = NULL;
}

static void
md_mount_operation_class_init (MdMountOperationClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GMountOperationClass *mop_class = G_MOUNT_OPERATION_CLASS (klass);
    object_class->dispose = md_mount_operation_dispose;
    mop_class->ask_password = md_mount_operation_ask_password;
    mop_class->ask_question = md_mount_operation_ask_question;
}

static void
md_mount_operation_dispose (GObject *object)
{
    MdMountOperation *op = MD_MOUNT_OPERATION (object);

    if (op->parent)
    {
        g_object_unref (op->parent);
        op->parent = NULL;
    }

    G_OBJECT_CLASS (md_mount_operation_parent_class)->dispose (object);
}

static GMountOperation *
mount_operation_new (GtkWidget *parent)
{
    MdMountOperation *op;

    op = g_object_new (MD_TYPE_MOUNT_OPERATION, NULL);

    if (parent)
        op->parent = g_object_ref (parent);

    return G_MOUNT_OPERATION (op);
}

static void
password_dialog_response (MdPasswordDialog *dialog,
                          int               response,
                          GMountOperation  *op)
{
    const char *username, *domain, *password;
    gboolean anonymous;

    switch (response)
    {
        case GTK_RESPONSE_HELP:
            return;

        case GTK_RESPONSE_OK:
            if ((username = md_password_dialog_get_username (dialog)))
                g_mount_operation_set_username (op, username);

            if ((domain = md_password_dialog_get_domain (dialog)))
                g_mount_operation_set_domain (op, domain);

            if ((password = md_password_dialog_get_password (dialog)))
                g_mount_operation_set_password (op, password);

            anonymous = md_password_dialog_anon_selected (dialog);
            g_mount_operation_set_anonymous (op, anonymous);

            g_mount_operation_reply (op, G_MOUNT_OPERATION_HANDLED);
            break;

        default:
            g_mount_operation_reply (op, G_MOUNT_OPERATION_ABORTED);
            break;
    }

    g_signal_handlers_disconnect_by_func (dialog,
                                          (gpointer) password_dialog_response,
                                          op);
    gtk_widget_destroy (GTK_WIDGET (dialog));
    g_object_unref (op);
}

static void
md_mount_operation_ask_password (GMountOperation   *op,
                                 const char        *message,
                                 const char        *default_user,
                                 const char        *default_domain,
                                 GAskPasswordFlags  flags)
{
    GtkWidget *dialog;

    dialog = md_password_dialog_new (MD_MOUNT_OPERATION (op)->parent,
                                     message, default_user, FALSE);

    md_password_dialog_set_show_password (MD_PASSWORD_DIALOG (dialog),
                                          flags & G_ASK_PASSWORD_NEED_PASSWORD);
    md_password_dialog_set_show_username (MD_PASSWORD_DIALOG (dialog),
                                          flags & G_ASK_PASSWORD_NEED_USERNAME);
    md_password_dialog_set_show_domain (MD_PASSWORD_DIALOG (dialog),
                                        flags & G_ASK_PASSWORD_NEED_DOMAIN);
    md_password_dialog_set_show_anonymous (MD_PASSWORD_DIALOG (dialog),
                                           flags & G_ASK_PASSWORD_ANONYMOUS_SUPPORTED);
    if (default_domain)
        md_password_dialog_set_domain (MD_PASSWORD_DIALOG (dialog), default_domain);

    g_signal_connect (dialog, "response", G_CALLBACK (password_dialog_response), op);

    gtk_widget_show (GTK_WIDGET (dialog));
    g_object_ref (op);
}


static void
question_dialog_response (MdPasswordDialog *dialog,
                          int               response,
                          GMountOperation  *op)
{
    if (response >= 0)
    {
        g_mount_operation_set_choice (op, response);
        g_mount_operation_reply (op, G_MOUNT_OPERATION_HANDLED);
    }
    else
    {
        g_mount_operation_reply (op, G_MOUNT_OPERATION_ABORTED);
    }

    g_signal_handlers_disconnect_by_func (dialog,
                                          (gpointer) question_dialog_response,
                                          op);
    gtk_widget_destroy (GTK_WIDGET (dialog));
    g_object_unref (op);
}

static void
md_mount_operation_ask_question (GMountOperation *op,
                                 const char      *message,
                                 const char      *choices[])
{
    GtkWidget *dialog;
    GtkWidget *parent = NULL;

    if (MD_MOUNT_OPERATION (op)->parent)
        parent = gtk_widget_get_toplevel (MD_MOUNT_OPERATION (op)->parent);

    dialog = gtk_message_dialog_new (GTK_IS_WINDOW (parent) ? GTK_WINDOW (parent) : NULL,
                                     0, GTK_MESSAGE_QUESTION,
                                     GTK_BUTTONS_NONE,
                                     "%s", message);

    if (choices)
    {
        int i;
        for (i = g_strv_length ((char**) choices) - 1; i >= 0; --i)
            gtk_dialog_add_button (GTK_DIALOG (dialog), choices[i], i);
    }

    g_signal_connect (GTK_OBJECT(dialog), "response",
                      G_CALLBACK (question_dialog_response), op);

    gtk_widget_show (GTK_WIDGET (dialog));
    g_object_ref (op);
}

#endif /* HAVE_GIO */


/*************************************************************************/
/* MdFileOp
 */

#define MD_FILE_OP_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_FILE_OP, MdFileOpClass))
#define MD_IS_FILE_OP_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_FILE_OP))
#define MD_FILE_OP_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_FILE_OP, MdFileOpClass))

static void     md_file_op_dispose          (GObject    *object);
static void     md_file_op_abort            (MdAsyncOp  *op);

typedef struct MdFileOpClass MdFileOpClass;

struct MdFileOp {
    MdAsyncOp base;
    MdFileInfo *file_info;
    MdFileOpStatus status;
    GError *error;
};

struct MdFileOpClass {
    MdAsyncOpClass base_class;
};

G_DEFINE_TYPE (MdFileOp, md_file_op, MD_TYPE_ASYNC_OP)

static void
md_file_op_init (MdFileOp *op)
{
    op->file_info = NULL;
    op->status = MD_FILE_OP_STATUS_IN_PROGRESS;
    md_async_op_start (MD_ASYNC_OP (op));
}

static void
md_file_op_class_init (MdFileOpClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MdAsyncOpClass *ao_class = MD_ASYNC_OP_CLASS (klass);
    object_class->dispose = md_file_op_dispose;
    ao_class->abort = md_file_op_abort;
}

static void
md_file_op_dispose (GObject *object)
{
    MdFileOp *op = MD_FILE_OP (object);

    md_file_info_free (op->file_info);
    op->file_info = NULL;

    if (op->error)
        g_error_free (op->error);
    op->error = NULL;

    G_OBJECT_CLASS (md_file_op_parent_class)->dispose (object);
}


static void
md_file_op_abort (MdAsyncOp *ao)
{
    MdFileOp *op = MD_FILE_OP (ao);

    op->status = MD_FILE_OP_STATUS_CANCELLED;

    MD_ASYNC_OP_CLASS (md_file_op_parent_class)->abort (ao);
}


static void
md_file_op_finished (MdFileOp       *op,
                     MdFileOpStatus  status)
{
    op->status = status;
    md_async_op_finish (MD_ASYNC_OP (op));
}

static void
md_file_op_errorv (MdFileOp   *op,
                   GQuark      err_domain,
                   int         err_code,
                   const char *format,
                   va_list     args)
{
    char *message;

    message = g_strdup_vprintf (format ? format : "Internal error", args);

    g_set_error (&op->error, err_domain, err_code, "%s", message);
    md_file_op_finished (op, MD_FILE_OP_STATUS_ERROR);

    g_free (message);
}

static void G_GNUC_PRINTF(4,5)
md_file_op_error (MdFileOp   *op,
                  GQuark      err_domain,
                  int         err_code,
                  const char *format,
                  ...)
{
    va_list args;

    va_start (args, format);
    md_file_op_errorv (op, err_domain, err_code, format, args);
    va_end (args);
}

MdFileInfo *
md_file_op_get_file_info (MdFileOp *op)
{
    g_return_val_if_fail (MD_IS_FILE_OP (op), NULL);
    return op->file_info;
}

MdFileOpStatus
md_file_op_get_status (MdFileOp *op)
{
    g_return_val_if_fail (MD_IS_FILE_OP (op), MD_FILE_OP_STATUS_ERROR);
    return op->status;
}

GError *
md_file_op_get_error (MdFileOp *op)
{
    g_return_val_if_fail (MD_IS_FILE_OP (op), NULL);
    return op->error;
}


/*************************************************************************/
/* MdFileLoader
 */

#define MD_FILE_LOADER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_FILE_LOADER, MdFileLoaderClass))
#define MD_IS_FILE_LOADER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_FILE_LOADER))
#define MD_FILE_LOADER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_FILE_LOADER, MdFileLoaderClass))

typedef struct MdFileLoaderClass MdFileLoaderClass;

typedef enum {
    LOADING = 1,
    MOUNTING,
    LOADING_AGAIN
} LoadingStage;

struct MdFileLoader {
    MdFileOp base;
    char *local_file;
    gboolean is_tmp;
#ifdef HAVE_GIO
    GCancellable *cancellable;
    GFile *source;
    LoadingStage stage;
#endif
};

struct MdFileLoaderClass {
    MdFileOpClass base_class;
};

G_DEFINE_TYPE (MdFileLoader, md_file_loader, MD_TYPE_FILE_OP)

static void md_file_loader_dispose  (GObject    *object);
static void md_file_loader_abort    (MdAsyncOp  *ao);

static void
md_file_loader_class_init (MdFileLoaderClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MdAsyncOpClass *ao_class = MD_ASYNC_OP_CLASS (klass);
    object_class->dispose = md_file_loader_dispose;
    ao_class->abort = md_file_loader_abort;
}

static void
md_file_loader_init (MdFileLoader *loader)
{
    loader->local_file = NULL;
    loader->is_tmp = FALSE;
}

static void
md_file_loader_dispose (GObject *object)
{
    MdFileLoader *loader = MD_FILE_LOADER (object);

    if (loader->is_tmp && loader->local_file)
        g_unlink (loader->local_file);

    g_free (loader->local_file);
    loader->local_file = NULL;

    G_OBJECT_CLASS (md_file_loader_parent_class)->dispose (object);
}

#ifdef HAVE_GIO
static void
loader_cleanup (MdFileLoader *loader)
{
    if (loader->source)
    {
        g_object_unref (loader->source);
        loader->source = NULL;
    }

    if (loader->cancellable &&
        !g_cancellable_is_cancelled (loader->cancellable))
    {
        g_cancellable_cancel (loader->cancellable);
        g_object_unref (loader->cancellable);
        loader->cancellable = NULL;
    }
}
#endif

static void
md_file_loader_abort (MdAsyncOp *ao)
{
#ifdef HAVE_GIO
    loader_cleanup (MD_FILE_LOADER (ao));
#endif
    MD_ASYNC_OP_CLASS (md_file_loader_parent_class)->abort (ao);
}

#ifdef HAVE_GIO
static void
loading_progress (goffset  n_bytes,
                  goffset  total,
                  gpointer user_data)
{
    MdFileLoader *loader = user_data;
    char *text;
    double fraction;

    if (total > 0)
    {
        fraction = (double) n_bytes / total;
        text = g_strdup_printf ("Transferred %lu of %lu bytes",
                                (gulong) n_bytes, (gulong) total);
    }
    else
    {
        fraction = -1;
        text = g_strdup_printf ("Transferred %lu bytes",
                                (gulong) n_bytes);
    }

    md_async_op_progress (MD_ASYNC_OP (loader), text, fraction);

    g_free (text);
}

static void
loading_failed (MdFileLoader *loader,
                GError       *error)
{
    if (error->domain == G_IO_ERROR && error->code == G_IO_ERROR_CANCELLED)
    {
        md_file_op_finished (MD_FILE_OP (loader), MD_FILE_OP_STATUS_CANCELLED);
    }
    else if (error->domain == G_IO_ERROR && error->code == G_IO_ERROR_WOULD_RECURSE)
    {
        char *uri = g_file_get_uri (loader->source);
        md_file_op_error (MD_FILE_OP (loader), G_IO_ERROR, G_IO_ERROR_IS_DIRECTORY,
                          "%s is a directory", uri);
        g_free (uri);
    }
    else
    {
        MD_FILE_OP (loader)->error = error;
        error = NULL;
        md_file_op_finished (MD_FILE_OP (loader), MD_FILE_OP_STATUS_ERROR);
    }

    loader_cleanup (loader);
    if (error)
        g_error_free (error);
}

static void
loading_async_ready_callback_2 (GObject      *source,
                                GAsyncResult *res,
                                gpointer      user_data)
{
    GError *error = NULL;
    MdFileLoader *loader = user_data;

    g_return_if_fail (!loader->source || G_OBJECT (loader->source) == source);

    if (!loader->source ||
        md_file_op_get_status (MD_FILE_OP (loader)) != MD_FILE_OP_STATUS_IN_PROGRESS)
    {
        g_file_copy_finish (loader->source, res, NULL);
    }
    else if (g_file_copy_finish (loader->source, res, &error))
    {
        md_file_op_finished (MD_FILE_OP (loader), MD_FILE_OP_STATUS_SUCCESS);
        loader_cleanup (loader);
    }
    else
    {
        loading_failed (loader, error);
    }

    g_object_unref (loader);
}

static void
load_again (MdFileLoader *loader)
{
    GFile *dest;

    dest = g_file_new_for_path (loader->local_file);
    loader->stage = LOADING_AGAIN;
    g_file_copy_async (loader->source, dest, 0, G_PRIORITY_DEFAULT,
                       loader->cancellable,
                       loading_progress, loader,
                       loading_async_ready_callback_2,
                       g_object_ref (loader));

    g_object_unref (dest);
}

static void
mount_async_ready_callback (GObject      *source,
                            GAsyncResult *res,
                            gpointer      user_data)
{
    GError *error = NULL;
    MdFileLoader *loader = user_data;

    g_return_if_fail (!loader->source || G_OBJECT (loader->source) == source);

    if (!loader->source ||
        md_file_op_get_status (MD_FILE_OP (loader)) != MD_FILE_OP_STATUS_IN_PROGRESS)
    {
        g_file_mount_enclosing_volume_finish (loader->source, res, NULL);
    }
    else if (g_file_mount_enclosing_volume_finish (loader->source, res, &error))
    {
        load_again (loader);
    }
    else
    {
        loading_failed (loader, error);
    }

    g_object_unref (loader);
}

static void
mount_and_load_again (MdFileLoader *loader)
{
    GMountOperation *mount_operation;
    char *uri, *progress_text;

    mount_operation = mount_operation_new (NULL);

    g_file_mount_enclosing_volume (loader->source, 0,
                                   mount_operation,
                                   loader->cancellable,
                                   mount_async_ready_callback,
                                   g_object_ref (loader));
    loader->stage = MOUNTING;

    uri = g_file_get_uri (loader->source);
    progress_text = g_strdup_printf ("Mounting %s", uri);
    md_async_op_progress (MD_ASYNC_OP (loader), progress_text, -1);

    g_free (progress_text);
    g_free (uri);
    g_object_unref (mount_operation);
}

static void
loading_async_ready_callback_1 (GObject      *source,
                                GAsyncResult *res,
                                gpointer      user_data)
{
    GError *error = NULL;
    MdFileLoader *loader = user_data;

    g_return_if_fail (!loader->source || G_OBJECT (loader->source) == source);

    if (!loader->source ||
        md_file_op_get_status (MD_FILE_OP (loader)) != MD_FILE_OP_STATUS_IN_PROGRESS)
    {
        g_file_copy_finish (loader->source, res, NULL);
    }
    else if (g_file_copy_finish (loader->source, res, &error))
    {
        md_file_op_finished (MD_FILE_OP (loader), MD_FILE_OP_STATUS_SUCCESS);
        loader_cleanup (loader);
    }
    else if (error->domain == G_IO_ERROR && error->code == G_IO_ERROR_NOT_MOUNTED)
    {
        mount_and_load_again (loader);
        g_error_free (error);
    }
    else
    {
        loading_failed (loader, error);
    }

    g_object_unref (loader);
}

static MdFileLoader *
create_remote_loader (MdFileInfo  *file_info,
                      GError     **error)
{
    const char *uri;
    char *tmp_file;
    GFile *dest;
    MdFileLoader *loader;

    tmp_file = moo_tempnam ();
    MD_CHECK_ARG (tmp_file != NULL, error, NULL);

    uri = md_file_info_get_uri (file_info);
    MD_CHECK_ARG (uri != NULL, error, NULL);

    loader = g_object_new (MD_TYPE_FILE_LOADER, NULL);
    MD_FILE_OP (loader)->file_info = md_file_info_copy (file_info);
    loader->local_file = tmp_file;
    loader->is_tmp = TRUE;

    dest = g_file_new_for_path (tmp_file);
    loader->stage = LOADING;
    loader->source = g_file_new_for_uri (uri);
    loader->cancellable = g_cancellable_new ();
    g_file_copy_async (loader->source, dest, 0, G_PRIORITY_DEFAULT,
                       loader->cancellable,
                       loading_progress, loader,
                       loading_async_ready_callback_1, loader);

    g_object_unref (dest);

    /* one ref is owned by the file transfer, and will be removed
     * in loading_async_ready_callback */
    return g_object_ref (loader);
}
#endif

MdFileLoader *
md_file_loader_new (MdFileInfo  *file,
                    GError     **error)
{
    MD_CHECK_ARG (file != NULL, error, NULL);

    if (!md_file_info_is_local (file))
    {
#ifndef HAVE_GIO
        g_set_error (error, MD_FILE_ERROR,
                     MD_FILE_ERROR_UNSUPPORTED,
                     "Loading remote files is not supported");
        return NULL;
#else /* HAVE_GIO */
        return create_remote_loader (file, error);
#endif /* HAVE_GIO */
    }
    else
    {
        MdFileOp *op;
        MdFileLoader *loader;
        char *local_file;

        local_file = md_file_info_get_filename (file);
        MD_CHECK_ARG (local_file != NULL, error, NULL);

        op = g_object_new (MD_TYPE_FILE_LOADER, NULL);
        loader = MD_FILE_LOADER (op);

        op->file_info = md_file_info_copy (file);
        loader->local_file = local_file;
        loader->is_tmp = FALSE;

        md_file_op_finished (op, MD_FILE_OP_STATUS_SUCCESS);

        return loader;
    }
}

const char *
md_file_loader_get_filename (MdFileLoader *loader)
{
    g_return_val_if_fail (MD_IS_FILE_LOADER (loader), NULL);
    g_return_val_if_fail (loader->local_file != NULL, NULL);
    return loader->local_file;
}


/*************************************************************************/
/* MdFileSaver
 */

#define MD_FILE_SAVER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_FILE_SAVER, MdFileSaverClass))
#define MD_IS_FILE_SAVER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_FILE_SAVER))
#define MD_FILE_SAVER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_FILE_SAVER, MdFileSaverClass))

typedef struct MdFileSaverClass MdFileSaverClass;

struct MdFileSaver {
    MdFileOp base;
};

struct MdFileSaverClass {
    MdFileOpClass base_class;

    void (*write_data) (MdFileSaver *saver,
                        const char  *data,
                        gsize        len);
    void (*rewind)     (MdFileSaver *saver);
    void (*finish)     (MdFileSaver *saver);
    void (*error)      (MdFileSaver *saver);
};

G_DEFINE_TYPE (MdFileSaver, md_file_saver, MD_TYPE_FILE_OP)

static MdFileSaver *md_file_saver_local_new  (MdFileInfo        *dest,
                                              MdFileSaverFlags   flags,
                                              GError           **error);
#ifdef HAVE_GIO
static MdFileSaver *md_file_saver_remote_new (MdFileInfo        *dest,
                                              MdFileSaverFlags   flags,
                                              GError           **error);
#endif

static void
md_file_saver_class_init (G_GNUC_UNUSED MdFileSaverClass *klass)
{
}

static void
md_file_saver_init (G_GNUC_UNUSED MdFileSaver *saver)
{
}

MdFileSaver *
md_file_saver_new (MdFileInfo        *file,
                   MdFileSaverFlags   flags,
                   GError           **error)
{
    MD_CHECK_ARG (file != NULL, error, NULL);

    if (!md_file_info_is_local (file))
    {
#ifdef HAVE_GIO
        return md_file_saver_remote_new (file, flags, error);
#else
        g_set_error (error, MD_FILE_ERROR,
                     MD_FILE_ERROR_UNSUPPORTED,
                     "Saving remote files is not supported");
        return NULL;
#endif
    }
    else
    {
        return md_file_saver_local_new (file, flags, error);
    }
}

void
md_file_saver_write (MdFileSaver *saver,
                     const char  *data,
                     gsize        len)
{
    g_return_if_fail (MD_IS_FILE_SAVER (saver));
    g_return_if_fail (data != NULL);
    g_return_if_fail (MD_FILE_OP (saver)->status == MD_FILE_OP_STATUS_IN_PROGRESS);

    if (len)
        MD_FILE_SAVER_GET_CLASS (saver)->write_data (saver, data, len);
}

void
md_file_saver_rewind (MdFileSaver *saver)
{
    g_return_if_fail (MD_IS_FILE_SAVER (saver));
    g_return_if_fail (MD_FILE_OP (saver)->status == MD_FILE_OP_STATUS_IN_PROGRESS);

    MD_FILE_SAVER_GET_CLASS (saver)->rewind (saver);
}

void
md_file_saver_finish (MdFileSaver *saver)
{
    g_return_if_fail (MD_IS_FILE_SAVER (saver));
    g_return_if_fail (MD_FILE_OP (saver)->status == MD_FILE_OP_STATUS_IN_PROGRESS);

    MD_FILE_SAVER_GET_CLASS (saver)->finish (saver);
}

void G_GNUC_PRINTF(4,5)
md_file_saver_error (MdFileSaver *saver,
                     GQuark       err_domain,
                     int          err_code,
                     const char  *fmt,
                     ...)
{
    MdFileOp *op;
    va_list args;

    g_return_if_fail (MD_IS_FILE_SAVER (saver));

    op = MD_FILE_OP (saver);
    g_return_if_fail (op->status == MD_FILE_OP_STATUS_IN_PROGRESS);

    if (MD_FILE_SAVER_GET_CLASS (saver)->error)
        MD_FILE_SAVER_GET_CLASS (saver)->error (saver);

    va_start (args, fmt);
    md_file_op_errorv (op, err_domain, err_code, fmt, args);
    va_end (args);
}


static gboolean
write_data (int          fd,
            const char  *data,
            gsize        len,
            GError     **error)
{
    while (len > 0)
    {
        int n;

        errno = 0;

        if ((n = write (fd, data, len)) < 0 && errno != EAGAIN && errno != EINTR)
        {
            int err = errno;
            g_set_error (error, G_FILE_ERROR,
                         g_file_error_from_errno (err),
                         "%s", g_strerror (err));
            return FALSE;
        }

        if (n > 0)
            len -= n;
    }

    return TRUE;
}


/*************************************************************************/
/* MdFileSaverLocal
 */

#define MD_TYPE_FILE_SAVER_LOCAL        (md_file_saver_local_get_type ())

typedef struct MdFileSaverLocal MdFileSaverLocal;
typedef struct MdFileSaverLocalClass MdFileSaverLocalClass;

struct MdFileSaverLocal {
    MdFileSaver base;
    char *tmp_filename;
    char *original_filename;
    char *backup_filename;
    int fd;
    guint close_fd : 1;
};

struct MdFileSaverLocalClass {
    MdFileSaverClass base_class;
};

MOO_DEFINE_TYPE_STATIC (MdFileSaverLocal, md_file_saver_local, MD_TYPE_FILE_SAVER)

static void md_file_saver_local_write_data  (MdFileSaver    *saver,
                                             const char     *data,
                                             gsize           len);
static void md_file_saver_local_finish      (MdFileSaver    *saver);
static void md_file_saver_local_error       (MdFileSaver    *saver);
static void md_file_saver_local_rewind      (MdFileSaver    *saver);

static void
md_file_saver_local_class_init (MdFileSaverLocalClass *klass)
{
    MdFileSaverClass *saver_class = MD_FILE_SAVER_CLASS (klass);
    saver_class->write_data = md_file_saver_local_write_data;
    saver_class->finish = md_file_saver_local_finish;
    saver_class->error = md_file_saver_local_error;
    saver_class->rewind = md_file_saver_local_rewind;
}

static void
md_file_saver_local_init (MdFileSaverLocal *saver)
{
    saver->fd = -1;
    saver->close_fd = TRUE;
    saver->tmp_filename = NULL;
    saver->original_filename = NULL;
    saver->backup_filename = NULL;
}

static void
md_file_saver_local_cleanup (MdFileSaverLocal *saver)
{
    if (saver->close_fd && saver->fd != -1)
        close (saver->fd);
    saver->fd = -1;

    if (saver->tmp_filename)
    {
        g_unlink (saver->tmp_filename);
        g_free (saver->tmp_filename);
        saver->tmp_filename = NULL;
    }

    g_free (saver->original_filename);
    g_free (saver->backup_filename);
    saver->original_filename = NULL;
    saver->backup_filename = NULL;
}

static void
md_file_saver_local_error (MdFileSaver *fsaver)
{
    MdFileSaverLocal *saver = (MdFileSaverLocal*) fsaver;

    md_file_saver_local_cleanup (saver);

    MD_FILE_SAVER_CLASS (md_file_saver_local_parent_class)->error (fsaver);
}

static void
md_file_saver_local_write_data (MdFileSaver *fsaver,
                                const char  *data,
                                gsize        len)
{
    GError *error = NULL;
    MdFileSaverLocal *saver = (MdFileSaverLocal*) fsaver;

    if (!write_data (saver->fd, data, len, &error))
    {
        MD_FILE_OP (saver)->error = error;
        md_file_op_finished (MD_FILE_OP (saver), MD_FILE_OP_STATUS_ERROR);
        md_file_saver_local_cleanup (saver);
    }
}

static void
md_file_saver_local_rewind (MdFileSaver *fsaver)
{
    MdFileSaverLocal *saver = (MdFileSaverLocal*) fsaver;
    int result;

    errno = 0;

#ifdef __WIN32__
    result = g_win32_ftruncate (saver->fd, 0);
#else
    result = ftruncate (saver->fd, 0);
#endif

    if (result != 0)
    {
        int err = errno;
        md_file_op_error (MD_FILE_OP (saver), G_FILE_ERROR,
                          g_file_error_from_errno (err),
                          "%s", g_strerror (err));
        md_file_saver_local_cleanup (saver);
    }
}

static gboolean
md_file_saver_local_close (MdFileSaverLocal  *saver,
                           GError           **error)
{
  int res = 0;

  if (saver->tmp_filename)
    {
      /* We need to move the temp file to its final place,
       * and possibly create the backup file
       */

      if (saver->backup_filename)
	{
#ifdef HAVE_LINK
	  /* create original -> backup link, the original is then renamed over */
	  if (g_unlink (saver->backup_filename) != 0 &&
	      errno != ENOENT)
	    {
              int errsv = errno;

	      g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
			   _("Error removing old backup link: %s"),
			   g_strerror (errsv));
	      goto err_out;
	    }

	  if (link (saver->original_filename, saver->backup_filename) != 0)
	    {
	      /*  link failed or is not supported, try rename  */
	      if (g_rename (saver->original_filename, saver->backup_filename) != 0)
		{
                  int errsv = errno;

	    	  g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
			       _("Error creating backup copy: %s"),
			       g_strerror (errsv));
	          goto err_out;
		}
	    }
#else
	    /* If link not supported, just rename... */
	  if (g_rename (saver->original_filename, saver->backup_filename) != 0)
	    {
              int errsv = errno;

	      g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
			   _("Error creating backup copy: %s"),
			   g_strerror (errsv));
	      goto err_out;
	    }
#endif
	}

      /* tmp -> original */
      if (g_rename (saver->tmp_filename, saver->original_filename) != 0)
	{
          int errsv = errno;

	  g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		       _("Error renaming temporary file: %s"),
		       g_strerror (errsv));
	  goto err_out;
	}
    }

  while (saver->close_fd)
    {
      res = close (saver->fd);
      if (res == -1)
	{
          int errsv = errno;

	  g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		       _("Error closing file: %s"),
		       g_strerror (errsv));
	}
      break;
    }

  return res != -1;

 err_out:
  if (saver->close_fd && saver->fd != -1)
  /* A simple try to close the fd in case we fail before the actual close */
    close (saver->fd);
  saver->fd = -1;
  return FALSE;
}

static void
md_file_saver_local_finish (MdFileSaver *fsaver)
{
    GError *error = NULL;
    MdFileSaverLocal *saver = (MdFileSaverLocal*) fsaver;

    if (md_file_saver_local_close (saver, &error))
    {
        md_file_op_finished (MD_FILE_OP (saver), MD_FILE_OP_STATUS_SUCCESS);
    }
    else
    {
        MD_FILE_OP (saver)->error = error;
        md_file_op_finished (MD_FILE_OP (saver), MD_FILE_OP_STATUS_ERROR);
    }

    md_file_saver_local_cleanup (saver);
}

static char *
create_backup_filename (const char *filename)
{
    return g_strconcat (filename, "~", NULL);
}

#define BUFSIZE	8192 /* size of normal write buffer */

static gboolean
copy_file_data (gint     sfd,
		gint     dfd,
		GError **error)
{
  gboolean ret = TRUE;
  gpointer buffer;
  const gchar *write_buffer;
  gssize bytes_read;
  gssize bytes_to_write;
  gssize bytes_written;

  buffer = g_malloc (BUFSIZE);

  do
    {
      bytes_read = read (sfd, buffer, BUFSIZE);
      if (bytes_read == -1)
	{
          int errsv = errno;

	  if (errsv == EINTR)
	    continue;

	  g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		       _("Error reading from file: %s"),
		       g_strerror (errsv));
	  ret = FALSE;
	  break;
	}

      bytes_to_write = bytes_read;
      write_buffer = buffer;

      do
	{
	  bytes_written = write (dfd, write_buffer, bytes_to_write);
	  if (bytes_written == -1)
	    {
              int errsv = errno;

	      if (errsv == EINTR)
		continue;

	      g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
			   _("Error writing to file: %s"),
			   g_strerror (errsv));
	      ret = FALSE;
	      break;
	    }

	  bytes_to_write -= bytes_written;
	  write_buffer += bytes_written;
	}
      while (bytes_to_write > 0);

    } while ((bytes_read != 0) && (ret == TRUE));

  g_free (buffer);

  return ret;
}

static int
handle_overwrite_open (const char    *filename,
		       gboolean       create_backup,
		       char         **temp_filename,
		       GError       **error)
{
  int fd = -1;
  struct stat original_stat;
  gboolean is_symlink;
  int open_flags;

  /* We only need read access to the original file if we are creating a backup.
   * We also add O_CREATE to avoid a race if the file was just removed */
  if (create_backup)
    open_flags = O_RDWR | O_CREAT;
  else
    open_flags = O_WRONLY | O_CREAT;

  /* Some systems have O_NOFOLLOW, which lets us avoid some races
   * when finding out if the file we opened was a symlink */
#ifdef O_NOFOLLOW
  is_symlink = FALSE;
  fd = g_open (filename, open_flags | O_NOFOLLOW, 0666);
  if (fd == -1 && errno == ELOOP)
    {
      /* Could be a symlink, or it could be a regular ELOOP error,
       * but then the next open will fail too. */
      is_symlink = TRUE;
      fd = g_open (filename, open_flags, 0666);
    }
#else
  fd = g_open (filename, open_flags, 0666);
  /* This is racy, but we do it as soon as possible to minimize the race */
  is_symlink = g_file_test (filename, G_FILE_TEST_IS_SYMLINK);
#endif

  if (fd == -1)
    {
      int errsv = errno;
      char *display_name = g_filename_display_name (filename);
      g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		   _("Error opening file '%s': %s"),
		   display_name, g_strerror (errsv));
      g_free (display_name);
      return -1;
    }

  if (fstat (fd, &original_stat) != 0)
    {
      int errsv = errno;
      char *display_name = g_filename_display_name (filename);
      g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		   _("Error stating file '%s': %s"),
		   display_name, g_strerror (errsv));
      g_free (display_name);
      goto err_out;
    }

  /* not a regular file */
  if (!S_ISREG (original_stat.st_mode))
    {
      if (S_ISDIR (original_stat.st_mode))
	g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		     _("Target file is a directory"));
      else
	g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		     _("Target file is not a regular file"));
      goto err_out;
    }

  /* We use two backup strategies.
   * The first one (which is faster) consist in saving to a
   * tmp file then rename the original file to the backup and the
   * tmp file to the original name. This is fast but doesn't work
   * when the file is a link (hard or symbolic) or when we can't
   * write to the current dir or can't set the permissions on the
   * new file.
   * The second strategy consist simply in copying the old file
   * to a backup file and rewrite the contents of the file.
   */

  if (!(original_stat.st_nlink > 1) && !is_symlink)
    {
      char *dirname, *tmp_filename;
      int tmpfd;

      dirname = g_path_get_dirname (filename);
      tmp_filename = g_build_filename (dirname, ".goutputstream-XXXXXX", NULL);
      g_free (dirname);

      tmpfd = g_mkstemp (tmp_filename);
      if (tmpfd == -1)
	{
	  g_free (tmp_filename);
	  goto fallback_strategy;
	}

      /* try to keep permissions */

      if (
#ifdef HAVE_FCHOWN
	  fchown (tmpfd, original_stat.st_uid, original_stat.st_gid) == -1 ||
#endif
#ifdef HAVE_FCHMOD
	  fchmod (tmpfd, original_stat.st_mode) == -1 ||
#endif
	  0
	  )
	{
	  struct stat tmp_statbuf;

	  /* Check that we really needed to change something */
	  if (fstat (tmpfd, &tmp_statbuf) != 0 ||
	      original_stat.st_uid != tmp_statbuf.st_uid ||
	      original_stat.st_gid != tmp_statbuf.st_gid ||
	      original_stat.st_mode != tmp_statbuf.st_mode)
	    {
	      close (tmpfd);
	      g_unlink (tmp_filename);
	      g_free (tmp_filename);
	      goto fallback_strategy;
	    }
	}

      close (fd);
      *temp_filename = tmp_filename;
      return tmpfd;
    }

 fallback_strategy:

  if (create_backup)
    {
      char *backup_filename;
      int bfd;

      backup_filename = create_backup_filename (filename);

      if (g_unlink (backup_filename) == -1 && errno != ENOENT)
	{
	  g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		       _("Backup file creation failed"));
	  g_free (backup_filename);
	  goto err_out;
	}

      bfd = g_open (backup_filename,
		    O_WRONLY | O_CREAT | O_EXCL,
		    original_stat.st_mode & 0777);

      if (bfd == -1)
	{
	  g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		       _("Backup file creation failed"));
	  g_free (backup_filename);
	  goto err_out;
	}

      /* If needed, Try to set the group of the backup same as the
       * original file. If this fails, set the protection
       * bits for the group same as the protection bits for
       * others. */
#if defined(HAVE_FCHOWN) && defined(HAVE_FCHMOD)
      {
        struct stat tmp_statbuf;

        if (fstat (bfd, &tmp_statbuf) != 0)
          {
            g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                         _("Backup file creation failed"));
            g_unlink (backup_filename);
            g_free (backup_filename);
            goto err_out;
          }

        if ((original_stat.st_gid != tmp_statbuf.st_gid)  &&
            fchown (bfd, (uid_t) -1, original_stat.st_gid) != 0)
          {
            if (fchmod (bfd,
                        (original_stat.st_mode & 0707) |
                        ((original_stat.st_mode & 07) << 3)) != 0)
              {
                g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                             _("Backup file creation failed"));
                g_unlink (backup_filename);
                close (bfd);
                g_free (backup_filename);
                goto err_out;
              }
          }
      }
#endif

      if (!copy_file_data (fd, bfd, NULL))
	{
	  g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		       _("Backup file creation failed"));
	  g_unlink (backup_filename);
	  close (bfd);
	  g_free (backup_filename);

	  goto err_out;
	}

      close (bfd);
      g_free (backup_filename);

      /* Seek back to the start of the file after the backup copy */
      if (lseek (fd, 0, SEEK_SET) == -1)
	{
          int errsv = errno;

	  g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		       _("Error seeking in file: %s"),
		       g_strerror (errsv));
	  goto err_out;
	}
    }

  /* Truncate the file at the start */
#ifdef G_OS_WIN32
  if (g_win32_ftruncate (fd, 0) == -1)
#else
  if (ftruncate (fd, 0) == -1)
#endif
    {
      int errsv = errno;

      g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
		   _("Error truncating file: %s"),
		   g_strerror (errsv));
      goto err_out;
    }

  return fd;

 err_out:
  close (fd);
  return -1;
}

static MdFileSaver *
md_file_saver_local_new (MdFileInfo        *file,
                         MdFileSaverFlags   flags,
                         GError           **error)
{
    MdFileSaverLocal *saver;
    gboolean create_backup = (flags & MD_FILE_SAVER_CREATE_BACKUP) != 0;
    int fd;
    char *filename;
    char *temp_file = NULL;

    filename = md_file_info_get_filename (file);
    MD_CHECK_ARG (filename != NULL, error, NULL);

    /* If the file doesn't exist, create it */
    fd = g_open (filename, O_CREAT | O_EXCL | O_WRONLY | O_BINARY, 0600);

    if (fd == -1 && errno == EEXIST)
    {
        /* The file already exists */
        fd = handle_overwrite_open (filename, create_backup, &temp_file, error);
        if (fd == -1)
            return NULL;
    }
    else if (fd == -1)
    {
        int errsv = errno;

        if (errsv == EINVAL)
        {
            /* This must be an invalid filename, on e.g. FAT */
            g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                         _("Invalid filename"));
        }
        else
        {
            char *display_name = g_filename_display_name (filename);
            g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                         _("Error opening file '%s': %s"),
                         filename, g_strerror (errsv));
            g_free (display_name);
        }

        return NULL;
    }

    saver = g_object_new (MD_TYPE_FILE_SAVER_LOCAL, NULL);
    MD_FILE_OP (saver)->file_info = md_file_info_copy (file);
    saver->fd = fd;
    saver->tmp_filename = temp_file;
    if (create_backup)
        saver->backup_filename = create_backup_filename (filename);
    saver->original_filename = filename;

    return MD_FILE_SAVER (saver);
}


/*************************************************************************/
/* MdFileSaverRemote
 */

#ifdef HAVE_GIO

#define MD_TYPE_FILE_SAVER_REMOTE       (md_file_saver_remote_get_type ())

typedef struct MdFileSaverRemote MdFileSaverRemote;
typedef struct MdFileSaverRemoteClass MdFileSaverRemoteClass;

typedef enum {
    REMOTE_SAVER_GETTING_DATA = 1,
    REMOTE_SAVER_OPENING,
    REMOTE_SAVER_MOUNTING,
    REMOTE_SAVER_OPENING_AGAIN,
    REMOTE_SAVER_WRITING,
    REMOTE_SAVER_CLOSING
} RemoteSaverStage;

struct MdFileSaverRemote {
    MdFileSaver base;
    RemoteSaverStage stage;
    char *tmp_file;
    int fd;
    GFile *dest;
    GCancellable *cancellable;
    GOutputStream *out_stream;
    GMappedFile *source;
    gsize bytes_written;
    guint create_backup : 1;
};

struct MdFileSaverRemoteClass {
    MdFileSaverClass base_class;
};

MOO_DEFINE_TYPE_STATIC (MdFileSaverRemote, md_file_saver_remote, MD_TYPE_FILE_SAVER)

static void md_file_saver_remote_abort          (MdAsyncOp      *ao);
static void md_file_saver_remote_write_data     (MdFileSaver    *saver,
                                                 const char     *data,
                                                 gsize           len);
static void md_file_saver_remote_finish         (MdFileSaver    *saver);
static void md_file_saver_remote_error          (MdFileSaver    *saver);
static void md_file_saver_remote_rewind         (MdFileSaver    *saver);

static void remote_saver_replace_async_ready    (GObject        *obj,
                                                 GAsyncResult   *res,
                                                 gpointer        user_data);
static void write_chunk                         (MdFileSaverRemote *saver);

static void
md_file_saver_remote_class_init (MdFileSaverRemoteClass *klass)
{
    MdAsyncOpClass *ao_class = MD_ASYNC_OP_CLASS (klass);
    MdFileSaverClass *saver_class = MD_FILE_SAVER_CLASS (klass);
    ao_class->abort = md_file_saver_remote_abort;
    saver_class->write_data = md_file_saver_remote_write_data;
    saver_class->finish = md_file_saver_remote_finish;
    saver_class->error = md_file_saver_remote_error;
    saver_class->rewind = md_file_saver_remote_rewind;
}

static void
md_file_saver_remote_init (MdFileSaverRemote *saver)
{
    saver->fd = -1;
}


static void
md_file_saver_remote_cleanup (MdFileSaverRemote *saver)
{
    saver->stage = 0;

    if (saver->fd != -1)
    {
        close (saver->fd);
        saver->fd = -1;
    }

    if (saver->source)
    {
        g_mapped_file_free (saver->source);
        saver->source = NULL;
    }

    if (saver->tmp_file)
    {
        g_unlink (saver->tmp_file);
        saver->tmp_file = NULL;
    }

    if (saver->dest)
    {
        g_object_unref (saver->dest);
        saver->dest = NULL;
    }

    if (saver->cancellable)
    {
        g_object_unref (saver->cancellable);
        saver->cancellable = NULL;
    }

    if (saver->out_stream)
    {
//         cancel_and_close_stream (saver->out_stream);
        g_object_unref (saver->out_stream);
        saver->out_stream = NULL;
    }
}


static void
md_file_saver_remote_abort (MdAsyncOp *ao)
{
    MdFileSaverRemote *saver = (MdFileSaverRemote*) ao;

    g_message ("%s", G_STRFUNC);

    if (saver->stage != REMOTE_SAVER_GETTING_DATA)
        g_cancellable_cancel (saver->cancellable);

    md_file_saver_remote_cleanup (saver);

    MD_ASYNC_OP_CLASS (md_file_saver_parent_class)->abort (ao);
}


static void
md_file_saver_remote_error (MdFileSaver *fsaver)
{
    MdFileSaverRemote *saver = (MdFileSaverRemote*) fsaver;

    g_return_if_fail (saver->stage == REMOTE_SAVER_GETTING_DATA);

    md_file_saver_remote_cleanup (saver);
}


static void
md_file_saver_remote_write_data (MdFileSaver *fsaver,
                                 const char  *data,
                                 gsize        len)
{
    MdFileSaverRemote *saver = (MdFileSaverRemote*) fsaver;
    GError *error = NULL;

    g_message ("%s", G_STRFUNC);

    if (saver->stage != REMOTE_SAVER_GETTING_DATA)
    {
        g_critical ("oops");
        md_file_op_error (MD_FILE_OP (saver), MD_FILE_ERROR,
                          MD_FILE_ERROR_FAILED, "Internal error");
        md_file_saver_remote_cleanup (saver);
        return;
    }

    if (!write_data (saver->fd, data, len, &error))
    {
        MD_FILE_OP (saver)->error = error;
        md_file_op_finished (MD_FILE_OP (saver), MD_FILE_OP_STATUS_ERROR);
        md_file_saver_remote_cleanup (saver);
    }
}

static void
saving_failed (MdFileSaverRemote *saver,
               GError            *error)
{
    if (error->domain == G_IO_ERROR && error->code == G_IO_ERROR_CANCELLED)
    {
        md_file_op_finished (MD_FILE_OP (saver), MD_FILE_OP_STATUS_CANCELLED);
        g_error_free (error);
    }
    else
    {
        MD_FILE_OP (saver)->error = error;
        md_file_op_finished (MD_FILE_OP (saver), MD_FILE_OP_STATUS_ERROR);
    }

    md_file_saver_remote_cleanup (saver);
}

static void
close_stream_async_ready (GObject      *obj,
                                GAsyncResult *res,
                                gpointer      user_data)
{
    MdFileSaverRemote *saver = user_data;
    GError *error = NULL;

    g_return_if_fail (!saver->out_stream || G_OBJECT (saver->out_stream) == obj);

    g_message ("%s", G_STRFUNC);

    if (md_file_op_get_status (MD_FILE_OP (saver)) != MD_FILE_OP_STATUS_IN_PROGRESS)
    {
        g_output_stream_close_finish (saver->out_stream, res, NULL);
    }
    else if (!g_output_stream_close_finish (saver->out_stream, res, &error))
    {
        saving_failed (saver, error);
    }
    else
    {
        g_object_unref (saver->out_stream);
        saver->out_stream = NULL;
        md_file_op_finished (MD_FILE_OP (saver), MD_FILE_OP_STATUS_SUCCESS);
        md_file_saver_remote_cleanup (saver);
    }

    g_object_unref (saver);
}

static void
close_stream (MdFileSaverRemote *saver)
{
    saver->stage = REMOTE_SAVER_CLOSING;
    g_output_stream_close_async (saver->out_stream, G_PRIORITY_DEFAULT,
                                 saver->cancellable,
                                 close_stream_async_ready,
                                 g_object_ref (saver));
}

static void
remote_saver_write_async_ready (GObject      *obj,
                                GAsyncResult *res,
                                gpointer      user_data)
{
    int n;
    MdFileSaverRemote *saver = user_data;
    GError *error = NULL;

    g_return_if_fail (!saver->out_stream || G_OBJECT (saver->out_stream) == obj);

    g_message ("%s", G_STRFUNC);

    if (md_file_op_get_status (MD_FILE_OP (saver)) != MD_FILE_OP_STATUS_IN_PROGRESS)
    {
        g_output_stream_write_finish (saver->out_stream, res, NULL);
    }
    else if ((n = g_output_stream_write_finish (saver->out_stream, res, &error)) < 0)
    {
        saving_failed (saver, error);
    }
    else
    {
        saver->bytes_written += n;
        write_chunk (saver);
    }

    g_object_unref (saver);
}

static void
write_chunk (MdFileSaverRemote *saver)
{
    gsize total;

    total = g_mapped_file_get_length (saver->source);

    g_message ("%s", G_STRFUNC);

    if (saver->bytes_written >= total)
        close_stream (saver);
    else
        g_output_stream_write_async (saver->out_stream,
                                     g_mapped_file_get_contents (saver->source) + saver->bytes_written,
                                     total - saver->bytes_written,
                                     G_PRIORITY_DEFAULT,
                                     saver->cancellable,
                                     remote_saver_write_async_ready,
                                     g_object_ref (saver));
}

static void
start_writing (MdFileSaverRemote *saver)
{
    GError *error = NULL;

    if (!(saver->source = g_mapped_file_new (saver->tmp_file, FALSE, &error)))
    {
        MD_FILE_OP (saver)->error = error;
        md_file_op_finished (MD_FILE_OP (saver), MD_FILE_OP_STATUS_ERROR);
        md_file_saver_remote_cleanup (saver);
    }
    else
    {
        saver->stage = REMOTE_SAVER_WRITING;
        saver->bytes_written = 0;
        write_chunk (saver);
    }
}

static void
save_again (MdFileSaverRemote *saver)
{
    saver->stage = REMOTE_SAVER_OPENING_AGAIN;
    g_file_replace_async (saver->dest, NULL, saver->create_backup, 0,
                          G_PRIORITY_DEFAULT,
                          saver->cancellable,
                          remote_saver_replace_async_ready,
                          g_object_ref (saver));
}

static void
remote_saver_mount_async_ready (GObject      *source,
                                GAsyncResult *res,
                                gpointer      user_data)
{
    GError *error = NULL;
    MdFileSaverRemote *saver = user_data;

    g_return_if_fail (!saver->dest || G_OBJECT (saver->dest) == source);

    g_message ("%s", G_STRFUNC);

    if (md_file_op_get_status (MD_FILE_OP (saver)) != MD_FILE_OP_STATUS_IN_PROGRESS)
    {
        g_file_mount_enclosing_volume_finish (saver->dest, res, NULL);
    }
    else if (g_file_mount_enclosing_volume_finish (saver->dest, res, &error))
    {
        save_again (saver);
    }
    else
    {
        saving_failed (saver, error);
    }

    g_object_unref (saver);
}

static void
mount_and_save_again (MdFileSaverRemote *saver)
{
    GMountOperation *mount_operation;
    char *uri, *progress_text;

    mount_operation = mount_operation_new (NULL);

    g_file_mount_enclosing_volume (saver->dest, 0,
                                   mount_operation,
                                   saver->cancellable,
                                   remote_saver_mount_async_ready,
                                   g_object_ref (saver));
    saver->stage = REMOTE_SAVER_MOUNTING;

    uri = g_file_get_uri (saver->dest);
    progress_text = g_strdup_printf ("Mounting %s", uri);
    md_async_op_progress (MD_ASYNC_OP (saver), progress_text, -1);

    g_free (progress_text);
    g_free (uri);
    g_object_unref (mount_operation);
}

static void
remote_saver_replace_async_ready (GObject      *obj,
                                  GAsyncResult *res,
                                  gpointer      user_data)
{
    MdFileSaverRemote *saver = user_data;

    g_return_if_fail (!saver->dest || G_OBJECT (saver->dest) == obj);

    g_message ("%s", G_STRFUNC);

    if (md_file_op_get_status (MD_FILE_OP (saver)) == MD_FILE_OP_STATUS_IN_PROGRESS)
    {
        GError *error = NULL;
        GFileOutputStream *out_stream;

        out_stream = g_file_replace_finish (saver->dest, res, &error);

        if (!out_stream)
        {
            if (saver->stage == REMOTE_SAVER_OPENING &&
                error->domain == G_IO_ERROR &&
                error->code == G_IO_ERROR_NOT_MOUNTED)
            {
                mount_and_save_again (saver);
                g_error_free (error);
            }
            else
            {
                saving_failed (saver, error);
            }
        }
        else
        {
            saver->out_stream = G_OUTPUT_STREAM (out_stream);
            start_writing (saver);
        }
    }

    g_object_unref (saver);
}

static void
md_file_saver_remote_finish (MdFileSaver *fsaver)
{
    MdFileInfo *file_info;
    MdFileSaverRemote *saver = (MdFileSaverRemote*) fsaver;

    g_message ("%s", G_STRFUNC);

    if (saver->stage != REMOTE_SAVER_GETTING_DATA)
    {
        g_critical ("oops");
        md_file_op_error (MD_FILE_OP (saver), MD_FILE_ERROR,
                          MD_FILE_ERROR_FAILED, "Internal error");
        md_file_saver_remote_cleanup (saver);
        return;
    }

    errno = 0;

    if (close (saver->fd) != 0)
    {
        int err = errno;
        md_file_op_error (MD_FILE_OP (saver), G_FILE_ERROR,
                          g_file_error_from_errno (err),
                          "%s", g_strerror (err));
        saver->fd = -1;
        md_file_saver_remote_cleanup (saver);
        return;
    }

    saver->stage = REMOTE_SAVER_OPENING;
    file_info = md_file_op_get_file_info (MD_FILE_OP (saver));
    saver->dest = g_file_new_for_uri (md_file_info_get_uri (file_info));
    saver->cancellable = g_cancellable_new ();

    g_file_replace_async (saver->dest, NULL, saver->create_backup, 0,
                          G_PRIORITY_DEFAULT,
                          saver->cancellable,
                          remote_saver_replace_async_ready,
                          g_object_ref (saver));
}

static void
md_file_saver_remote_rewind (MdFileSaver *fsaver)
{
    int result;
    MdFileSaverRemote *saver = (MdFileSaverRemote*) fsaver;

    g_message ("%s", G_STRFUNC);

    if (saver->stage != REMOTE_SAVER_GETTING_DATA)
    {
        g_critical ("oops");
        md_file_op_error (MD_FILE_OP (saver), MD_FILE_ERROR,
                          MD_FILE_ERROR_FAILED, "Internal error");
        md_file_saver_remote_cleanup (saver);
        return;
    }

    errno = 0;

#ifdef __WIN32__
    result = g_win32_ftruncate (saver->fd, 0);
#else
    result = ftruncate (saver->fd, 0);
#endif

    if (result != 0)
    {
        int err = errno;
        md_file_op_error (MD_FILE_OP (saver), G_FILE_ERROR,
                          g_file_error_from_errno (err),
                          "%s", g_strerror (err));
        saver->fd = -1;
        md_file_saver_remote_cleanup (saver);
    }
}

static MdFileSaver *
md_file_saver_remote_new (MdFileInfo        *file,
                          MdFileSaverFlags   flags,
                          GError           **error)
{
    MdFileSaverRemote *saver;
    char *tmp_file;
    int fd;

    tmp_file = moo_tempnam ();
    MD_CHECK_ARG (tmp_file != NULL, error, NULL);

    fd = md_open_file_for_writing (tmp_file, error);
    if (fd == -1)
    {
        g_free (tmp_file);
        return NULL;
    }

    saver = g_object_new (MD_TYPE_FILE_SAVER_REMOTE, NULL);
    MD_FILE_OP (saver)->file_info = md_file_info_copy (file);
    saver->tmp_file = tmp_file;
    saver->fd = fd;
    saver->create_backup = (flags & MD_FILE_SAVER_CREATE_BACKUP) != 0;
    saver->stage = REMOTE_SAVER_GETTING_DATA;

    return MD_FILE_SAVER (saver);
}

#endif /* HAVE_GIO */
