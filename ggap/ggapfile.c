/*
 *   ggapfile.c
 *
 *   Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "ggapfile.h"
#include <stdio.h>
#include <zlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <glib/gstdio.h>
#include <mooutils/mooutils-misc.h>

#define GGAP_FILE_VERSION       "0001"
#define GGAP_FILE_VERSION_LEN   4
#define GGAP_FILE_MAGIC         "ggap\0\1\2\3"
#define GGAP_FILE_MAGIC_LEN     8
#define GGAP_FILE_HEADER        GGAP_FILE_MAGIC GGAP_FILE_VERSION
#define GGAP_FILE_HEADER_LEN    (GGAP_FILE_MAGIC_LEN + GGAP_FILE_VERSION_LEN)

/*
 * Packed file is magic bytes followed by data compressed
 * with zlib-compressed data:
 *
 * N_FILES FILE1 FILE2...
 * FILE: FILE_LENGTH FILE_CONTENTS
 * N_FILES, FILE_LENGTH: guint32
 *
 * Signature bytes are not compressed to make detecting
 * content type possible for xdgmime.
 */

static gzFile
open_file_w (const char *filename,
             GError    **error)
{
    gzFile file;
    int fd;

    fd = g_open (filename, O_WRONLY | O_CREAT | O_TRUNC,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (fd == -1)
    {
        int err = errno;
        g_set_error (error, G_FILE_ERROR,
                     g_file_error_from_errno (err),
                     "could not open file %s for writing",
                     filename);
        return NULL;
    }

    /* XXX */
    if (write (fd, GGAP_FILE_HEADER, GGAP_FILE_HEADER_LEN) != GGAP_FILE_HEADER_LEN)
    {
        g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_FAILED,
                     "could not write magic");
        close (fd);
        return NULL;
    }

    file = gzdopen (fd, "ab1");

    if (!file)
    {
        g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_NOMEM,
                     "gzdopen failed for %s", filename);
        close (fd);
        g_unlink (filename);
        return NULL;
    }

    return file;
}

static gzFile
gap_file_open_w (const char *filename,
                 guint       n_files,
                 GError    **error)
{
    gzFile file;
    guint32 n_files_be;

    if (!(file = open_file_w (filename, error)))
        return NULL;

    n_files_be = GUINT32_TO_BE ((guint32) n_files);

    if (gzwrite (file, &n_files_be, 4) != 4)
    {
        g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_FAILED,
                     "could not write n_files");
        goto error;
    }

    return file;

error:
    if (error && !*error)
        g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_FAILED,
                     "could not save file %s", filename);
    gzclose (file);
    g_unlink (filename);
    return NULL;
}

static gboolean
gap_file_write (gzFile      file,
                const char *data,
                gsize       len,
                GError    **error)
{
    guint32 len_be;

    if (len > G_MAXUINT32)
    {
        g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_FAILED, "data too long");
        return FALSE;
    }

    len_be = GUINT32_TO_BE ((guint32) len);

    if (gzwrite (file, &len_be, 4) != 4)
    {
        g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_FAILED, "could not write length");
        return FALSE;
    }

    if (len > 0 && gzwrite (file, data, len) != (int) len)
    {
        g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_FAILED, "could not write data");
        return FALSE;
    }

    return TRUE;
}

gboolean
ggap_file_pack (const char *text,
                const char *binary_file,
                const char *filename,
                GError    **error)
{
    gzFile file;
    GMappedFile *mfile = NULL;
    guint n_files;

    g_return_val_if_fail (text != NULL, FALSE);
    g_return_val_if_fail (filename != NULL, FALSE);

    n_files = binary_file ? 2 : 1;

    if (!(file = gap_file_open_w (filename, n_files, error)))
        return FALSE;

    if (!gap_file_write (file, text, strlen (text), error))
        goto error;

    if (binary_file)
    {
        if (!(mfile = g_mapped_file_new (binary_file, FALSE, error)))
            goto error;

        if (!gap_file_write (file, g_mapped_file_get_contents (mfile),
                             g_mapped_file_get_length (mfile), error))
            goto error;
    }

    if (gzclose (file) == -1)
    {
        file = NULL;
        g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_FAILED,
                     "error in gzclose");
        goto error;
    }

    if (mfile)
        g_mapped_file_free (mfile);

    return TRUE;

error:
    g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_FAILED,
                 "could not save file %s", filename);
    if (file)
        gzclose (file);
    if (mfile)
        g_mapped_file_free (mfile);
    g_unlink (filename);
    return FALSE;
}


// static gboolean
// write_chars_stdio (FILE       *output,
//                    const void *data,
//                    gsize       data_len,
//                    GError    **error)
// {
//     while (data_len)
//     {
//         gsize written;
//
//         written = fwrite (data, 1, data_len, output);
//
//         if (written)
//         {
//             data_len -= written;
//             data = (char*) data + written;
//         }
//         else
//         {
//             g_set_error (error, GGAP_FILE_ERROR,
//                          GGAP_FILE_ERROR_STDIO,
//                          "could not write data");
//             return FALSE;
//         }
//     }
//
//     return TRUE;
// }
//
// static gboolean
// write_chars (z_stream   *stream,
//              const void *data,
//              gsize       n_bytes,
//              gboolean    finish,
//              FILE       *output,
//              GError    **error)
// {
//     guint8 compressed[2048];
//
//     stream->next_in = (void*) data;
//     stream->avail_in = n_bytes;
//
//     while (stream->avail_in)
//     {
//         stream->next_out = compressed;
//         stream->avail_out = sizeof compressed;
//
//         switch (deflate (stream, Z_NO_FLUSH))
//         {
//             case Z_OK:
//                 if (stream->next_out > compressed)
//                 {
//                     if (!write_chars_stdio (output, compressed,
//                                             stream->next_out - compressed,
//                                             error))
//                     {
//                         return FALSE;
//                     }
//                 }
//                 break;
//
//             default:
//                 g_set_error (error, GGAP_FILE_ERROR,
//                              GGAP_FILE_ERROR_ZLIB,
//                              "compression failed");
//                 return FALSE;
//         }
//     }
//
//     if (finish)
//     {
//         gboolean done = FALSE;
//
//         stream->next_in = NULL;
//         stream->avail_in = 0;
//
//         while (!done)
//         {
//             stream->next_out = compressed;
//             stream->avail_out = sizeof compressed;
//
//             switch (deflate (stream, Z_FINISH))
//             {
//                 case Z_OK:
//                     if (stream->next_out > compressed)
//                     {
//                         if (!write_chars_stdio (output, compressed,
//                                                 stream->next_out - compressed,
//                                                 error))
//                         {
//                             return FALSE;
//                         }
//                     }
//                     break;
//
//                 case Z_STREAM_END:
//                     done = TRUE;
//                     break;
//
//                 default:
//                     g_set_error (error, GGAP_FILE_ERROR,
//                                  GGAP_FILE_ERROR_ZLIB,
//                                  "compression failed");
//                     break;
//             }
//         }
//     }
//
//     return TRUE;
// }
//
// static gboolean
// shutdown_writer (z_stream **stream,
//                  FILE      *output,
//                  GError   **error)
// {
//     gboolean ret;
//
//     if (!*stream)
//         return TRUE;
//
//     if (output)
//         ret = write_chars (*stream, NULL, 0, TRUE, output, error);
//     else
//         ret = TRUE;
//
//     deflateEnd (*stream);
//     g_free (*stream);
//     *stream = NULL;
//     return ret;
// }
//
// static gboolean
// prepare_writer (z_stream **streamp,
//                 FILE      *output,
//                 guint      n_files,
//                 GError   **error)
// {
//     int ret;
//     z_stream *stream;
//
//     *streamp = NULL;
//
//     stream = g_new0 (z_stream, 1);
//
//     ret = deflateInit (stream, Z_DEFAULT_COMPRESSION);
//
//     if (ret == Z_OK)
//     {
//         guint32 n_files_be;
//
//         n_files_be = GUINT32_TO_BE ((guint32) n_files);
//
//         if (!write_chars (stream, &n_files_be, 4, FALSE, output, error))
//         {
//             shutdown_writer (&stream, NULL, NULL);
//             return FALSE;
//         }
//
//         *streamp = stream;
//         return TRUE;
//     }
//
//     g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_ZLIB,
//                  "could not initialize compression");
//     g_free (stream);
//     return FALSE;
// }
//
// static gboolean
// write_file_data (z_stream   *stream,
//                  const char *data,
//                  gsize       data_len,
//                  FILE       *output,
//                  GError    **error)
// {
//     guint32 len_be;
//
//     len_be = GUINT32_TO_BE ((guint32) data_len);
//
//     return write_chars (stream, &len_be, 4, FALSE, output, error) &&
//            write_chars (stream, data, data_len, FALSE, output, error);
// }
//
// gboolean
// ggap_file_pack (const char *text,
//                 const char *binary_file,
//                 const char *filename,
//                 GError    **error)
// {
//     z_stream *stream = NULL;
//     FILE *output = NULL;
//     GMappedFile *mfile = NULL;
//     guint n_files;
//
//     g_return_val_if_fail (text != NULL, FALSE);
//     g_return_val_if_fail (filename != NULL, FALSE);
//
//     if (!(output = g_fopen (filename, "wb")))
//     {
//         int err = errno;
//         g_set_error (error, G_FILE_ERROR,
//                      g_file_error_from_errno (err),
//                      "could not open file %s for writing: %s",
//                      filename, g_strerror (err));
//         return FALSE;
//     }
//
//     if (!write_chars_stdio (output, GGAP_FILE_HEADER, GGAP_FILE_HEADER_LEN, error))
//         goto error;
//
//     n_files = binary_file ? 2 : 1;
//
//     if (!prepare_writer (&stream, output, n_files, error))
//         goto error;
//
//     if (!write_file_data (stream, text, strlen(text), output, error))
//         goto error;
//
//     if (binary_file)
//     {
//         if (!(mfile = g_mapped_file_new (binary_file, FALSE, error)))
//             goto error;
//
//         if (!write_file_data (stream,
//                               g_mapped_file_get_contents (mfile),
//                               g_mapped_file_get_length (mfile),
//                               output,
//                               error))
//             goto error;
//     }
//
//     if (!shutdown_writer (&stream, output, error))
//         goto error;
//
//     if (fclose (output) != 0)
//     {
//         int err = errno;
//         g_set_error (error, G_FILE_ERROR,
//                      g_file_error_from_errno (err),
//                      "could not save file %s: %s",
//                      filename, g_strerror (err));
//         output = 0;
//         goto error;
//     }
//
//     if (mfile)
//         g_mapped_file_free (mfile);
//
//     return TRUE;
//
// error:
//     if (output)
//         fclose (output);
//     if (stream)
//         shutdown_writer (&stream, NULL, NULL);
//     if (mfile)
//         g_mapped_file_free (mfile);
//     g_unlink (filename);
//     return FALSE;
// }


static gzFile
open_file_r (const char *filename,
             GError    **error)
{
    gzFile file;
    int fd;
    char buf[GGAP_FILE_HEADER_LEN];

    fd = g_open (filename, O_RDONLY, 0);

    if (fd == -1)
    {
        int err = errno;
        g_set_error (error, G_FILE_ERROR,
                     g_file_error_from_errno (err),
                     "could not open file %s for reading",
                     filename);
        return NULL;
    }

    if (read (fd, buf, GGAP_FILE_HEADER_LEN) != GGAP_FILE_HEADER_LEN)
    {
        int err = errno;
        g_set_error (error, G_FILE_ERROR,
                     g_file_error_from_errno (err),
                     "could not read magic");
        close (fd);
        return NULL;
    }

    if (memcmp (buf, GGAP_FILE_HEADER, GGAP_FILE_HEADER_LEN) != 0)
    {
        if (memcmp (buf, GGAP_FILE_MAGIC, GGAP_FILE_MAGIC_LEN) == 0)
        {
            g_set_error (error, GGAP_FILE_ERROR,
                         GGAP_FILE_ERROR_BAD_VERSION,
                         "bad version");
        }
        else
        {
            g_set_error (error, GGAP_FILE_ERROR,
                         GGAP_FILE_ERROR_BAD_DATA,
                         "bad magic");
        }

        close (fd);
        return NULL;
    }

    file = gzdopen (fd, "rb");

    if (!file)
    {
        g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_NOMEM,
                     "could not open file %s", filename);
        close (fd);
        return NULL;
    }

    return file;
}

static gzFile
gap_file_open_r (const char *filename,
                 guint      *n_files,
                 GError    **error)
{
    gzFile file;
    guint32 n_files_be;

    if (!(file = open_file_r (filename, error)))
        return NULL;

    if (gzread (file, &n_files_be, 4) != 4)
        goto error;

    *n_files = GUINT32_FROM_BE (n_files_be);
    return file;

error:
    g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_FAILED,
                 "could not load file %s", filename);
    gzclose (file);
    return NULL;
}

static gboolean
gap_file_read_m (gzFile   file,
                 char   **text_p,
                 gsize   *text_len)
{
    char *buf;
    guint32 length;

    if (gzread (file, &length, 4) != 4)
        return FALSE;

    length = GUINT32_FROM_BE (length);
    buf = g_try_malloc (length);

    if (!buf)
        return FALSE;

    if (gzread (file, buf, length) != (int) length)
        return FALSE;

    *text_p = buf;
    *text_len = length;
    return TRUE;
}

static gboolean
gap_file_read_f (gzFile   file,
                 char   **filename_p)
{
    FILE *dest = NULL;
    char *filename = NULL;
    guint32 length;

    if (!(filename = moo_tempnam ()))
        goto error;

    if (!(dest = g_fopen (filename, "wb")))
        goto error;

    if (gzread (file, &length, 4) != 4)
        return FALSE;

    length = GUINT32_FROM_BE (length);

    while (length)
    {
        char buf[2048];
        gsize n_bytes = MIN (sizeof buf, length);

        if (gzread (file, buf, n_bytes) != (int) n_bytes)
            goto error;

        if (fwrite (buf, 1, n_bytes, dest) != n_bytes)
            goto error;

        length -= n_bytes;
    }

    if (fclose (dest) != 0)
    {
        dest = NULL;
        goto error;
    }

    *filename_p = filename;
    return TRUE;

error:
    if (dest)
        fclose (dest);
    if (filename)
        g_unlink (filename);
    g_free (filename);
    return FALSE;
}

static gboolean
gap_file_eof (gzFile file)
{
    if (gzeof (file))
        return TRUE;
    gzread (file, NULL, 0);
    if (gzeof (file))
        return TRUE;
    return FALSE;
}

gboolean
ggap_file_unpack (const char *filename,
                  char      **text_p,
                  gsize      *text_len_p,
                  char      **binary_file_p,
                  GError    **error)
{
    gzFile file;
    char *binary_file = NULL;
    char *text = NULL;
    gsize text_len;
    guint n_files;

    g_return_val_if_fail (filename != NULL, FALSE);
    g_return_val_if_fail (text_p != NULL, FALSE);
    g_return_val_if_fail (text_len_p != NULL, FALSE);
    g_return_val_if_fail (binary_file_p != NULL, FALSE);

    *text_p = NULL;
    *text_len_p = 0;
    *binary_file_p = NULL;

    if (!(file = gap_file_open_r (filename, &n_files, error)))
        return FALSE;

    if (n_files != 1 && n_files != 2)
        goto error;

    if (!gap_file_read_m (file, &text, &text_len))
        goto error;

    if (n_files == 2 && !gap_file_read_f (file, &binary_file))
        goto error;

    if (!gap_file_eof (file))
        goto error;

    *text_p = text;
    *text_len_p = text_len;
    *binary_file_p = binary_file;

    gzclose (file);
    return TRUE;

error:
    g_set_error (error, GGAP_FILE_ERROR, GGAP_FILE_ERROR_FAILED,
                 "could not load file %s", filename);
    if (file)
        gzclose (file);
    if (binary_file)
        g_unlink (binary_file);
    g_free (binary_file);
    g_free (text);
    return FALSE;
}


GQuark
ggap_file_error_quark (void)
{
    static GQuark q;

    if (G_UNLIKELY (!q))
        q = g_quark_from_static_string ("ggap-file-error");

    return q;
}
