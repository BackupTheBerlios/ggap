/*
 *   gap-script.h
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

#ifndef __GAP_SCRIPT_H__
#define __GAP_SCRIPT_H__

#include <gtk/gtktreemodel.h>

G_BEGIN_DECLS


#define GAP_DATA_NONE               '\0'    /* <\0> */
#define GAP_DATA_COMMAND            '\1'    /* <\1><SMALL_INT>(data) */
#define GAP_DATA_STRING             '\2'    /* <\2><len_high><len_low>(string bytes) - len_high == len/128; len_low == len%128 */
#define GAP_DATA_SMALL_INT          '\3'    /* <\3><single byte> */
#define GAP_DATA_LIST               '\4'    /* <\4><len_high><len_low>(first_elm)...(last_elm) */
#define GAP_DATA_BOOL               '\5'    /* <\5><SMALL_INT> */
#define GAP_DATA_INT                '\6'    /* <\6><pm><val_high><val_low> */
#define GAP_DATA_OBJECT             '\7'    /* <\7>(type)(id) */

#define GAP_COMMAND_EXEC_FILE       '\1'
#define GAP_COMMAND_SIGNAL          '\2'
#define GAP_COMMAND_OBJECT_DIED     '\3'
#define GAP_COMMAND_CLOSE_WINDOW    '\4'

#define GAP_STATUS_OK               0
#define GAP_STATUS_ERROR            1


/* frees data */
void     gap_data_send                      (GString        *data);

void     gap_data_add_string                (GString        *data,
                                             const char     *string,
                                             int             len);
void     gap_data_add_list                  (GString        *data,
                                             guint           len);
void     gap_data_add_int                   (GString        *data,
                                             int             val);
void     gap_data_add_tree_path             (GString        *data,
                                             GtkTreePath    *path);

void     gap_data_add_command_object_died   (GString        *data,
                                             const char     *id);
void     gap_data_add_command_exec_file     (GString        *data,
                                             const char     *filename);
void     gap_data_add_command_signal        (GString        *data,
                                             const char     *signal,
                                             guint           n_args);
void     gap_data_add_command_close_window  (GString        *data,
                                             const char     *window_id);

#define gap_data_add_small_int(data__, val__)       \
G_STMT_START {                                      \
    g_string_append_c (data__, GAP_DATA_SMALL_INT); \
    g_string_append_c (data__, val__);              \
} G_STMT_END

#define gap_data_add_bool(data__, val__)            \
G_STMT_START {                                      \
    g_string_append_c (data__, GAP_DATA_BOOL);      \
    g_string_append_c (data__, val__);              \
} G_STMT_END

#define gap_data_add_command(data__) g_string_append_c (data__, GAP_DATA_COMMAND)
#define gap_data_add_pair(data__) gap_data_add_list (data__, 2)
#define gap_data_add_triple(data__) gap_data_add_list (data__, 3)
#define gap_data_add_none(data__) g_string_append_c (data__, GAP_DATA_NONE)


G_END_DECLS

#endif /* __GAP_SCRIPT_H__ */
