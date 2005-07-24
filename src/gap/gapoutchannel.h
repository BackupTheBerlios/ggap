/*
 *   @(#)$Id: gapoutchannel.h,v 1.1 2005/04/22 08:49:55 emuntyan Exp $
 *
 *   Copyright (C) 2004-2005 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef GAP_GAPOUTPUT_H
#define GAP_GAPOUTPUT_H

#ifdef __WIN32__
#include <windows.h>
#endif // __WIN32__
#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

#define GAP_TYPE_OUT_CHANNEL              (gap_out_channel_get_type ())
#define GAP_OUT_CHANNEL(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), GAP_TYPE_OUT_CHANNEL, GapOutChannel))
#define GAP_OUT_CHANNEL_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_OUT_CHANNEL, GapOutChannelClass))
#define GAP_IS_OUT_CHANNEL(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), GAP_TYPE_OUT_CHANNEL))
#define GAP_IS_OUT_CHANNEL_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_OUT_CHANNEL))
#define GAP_OUT_CHANNEL_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_OUT_CHANNEL, GapOutChannelClass))

typedef struct _GapOutChannel       GapOutChannel;
typedef struct _GapOutChannelClass  GapOutChannelClass;

struct _GapOutChannel {
    GObject     parent;

#ifdef __WIN32__
    HANDLE pipe;
#else /* !__WIN32__ */
    int pipe;
#endif /* !__WIN32__ */
    char *pipe_name;
    GIOChannel *io;
    guint io_watch;
    guint ready : 1;
};

struct _GapOutChannelClass {
    GObjectClass  parent_class;

    void (*io_error) (GapOutChannel *ch);
};


GType           gap_out_channel_get_type    (void);

GapOutChannel  *gap_out_channel_new         (const char     *fifo_basename);

gboolean        gap_out_channel_start       (GapOutChannel  *ch);
void            gap_out_channel_shutdown    (GapOutChannel  *ch);
gboolean        gap_out_channel_restart     (GapOutChannel  *ch);
gboolean        gap_out_channel_ready       (GapOutChannel  *ch);

const char     *gap_out_channel_get_name    (GapOutChannel  *ch);

gboolean        gap_out_channel_write       (GapOutChannel  *ch,
                                             const char     *data,
                                             gssize          len);


G_END_DECLS

#endif /* GAP_GAPOUTPUT_H */
