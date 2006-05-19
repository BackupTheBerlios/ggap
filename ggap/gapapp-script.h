/*
 *   gapapp-script.h
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

#ifndef __GAP_APP_SCRIPT_H__
#define __GAP_APP_SCRIPT_H__

#include <glib.h>

G_BEGIN_DECLS


#define GAP_DATA_COMMAND            '\0'    /* <\0><SMALL_INT>(data) */
#define GAP_DATA_STRING             '\1'    /* <\1><len_high><len_low>(string bytes) - len_high == len/128; len_low == len%128 */
#define GAP_DATA_SMALL_INT          '\2'    /* <\2><single byte> */
#define GAP_DATA_PAIR               '\3'    /* <\3>(first)(second) */
#define GAP_DATA_TRIPLE             '\4'    /* <\4>(first)(second)(third) */
#define GAP_DATA_LIST               '\5'    /* <\5><SMALL_INT>(first_elm)...(last_elm) */
#define GAP_DATA_BOOL               '\6'    /* <\6><SMALL_INT> */
#define GAP_DATA_INT                '\7'    /* <\7><pm><val_high><val_low> */

#define GAP_COMMAND_EXEC_FILE       '\1'
#define GAP_COMMAND_SIGNAL          '\2'
#define GAP_COMMAND_OBJECT_DIED     '\3'
#define GAP_COMMAND_CLOSE_WINDOW    '\4'

#define GAP_STATUS_OK               0
#define GAP_STATUS_ERROR            1


void     gap_data_send                      (GString    *data);

void     gap_data_add_string                (GString    *data,
                                             const char *string,
                                             int         len);
void     gap_data_add_int                   (GString    *data,
                                             int         val);

void     gap_data_add_command_object_died   (GString    *data,
                                             const char *id);
void     gap_data_add_command_exec_file     (GString    *data,
                                             const char *filename);
void     gap_data_add_command_signal        (GString    *data,
                                             const char *object_id,
                                             const char *signal,
                                             guint       n_args);
void     gap_data_add_command_close_window  (GString    *data,
                                             const char *window_id);

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

#define gap_data_add_list(data__, len__)            \
G_STMT_START {                                      \
    g_string_append_c (data__, GAP_DATA_LIST);      \
    g_string_append_c (data__, len__);              \
} G_STMT_END

#define gap_data_add_command(data__) g_string_append_c (data__, GAP_DATA_COMMAND)
#define gap_data_add_pair(data__) g_string_append_c (data__, GAP_DATA_PAIR)
#define gap_data_add_triple(data__) g_string_append_c (data__, GAP_DATA_TRIPLE)


G_END_DECLS

#endif /* __GAP_APP_SCRIPT_H__ */
