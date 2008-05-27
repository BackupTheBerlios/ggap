/*
 *   gapprocess.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef GAP_PROCESS_H
#define GAP_PROCESS_H

#include <glib-object.h>

G_BEGIN_DECLS


#define GAP_TYPE_PROCESS             (gap_process_get_type ())
#define GAP_PROCESS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_PROCESS, GapProcess))
#define GAP_PROCESS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_PROCESS, GapProcessClass))
#define GAP_IS_PROCESS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_PROCESS))
#define GAP_IS_PROCESS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_PROCESS))
#define GAP_PROCESS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_PROCESS, GapProcessClass))

typedef struct GapProcess        GapProcess;
typedef struct GapProcessPrivate GapProcessPrivate;
typedef struct GapProcessClass   GapProcessClass;

typedef enum {
    GAP_DEAD = 0,
    GAP_IN_PROMPT,
    GAP_LOADING,
    GAP_BUSY,
    GAP_BUSY_INTERNAL
} GapState;

struct GapProcess
{
    GObject base;
    GapProcessPrivate *priv;
};

struct GapProcessClass
{
    GObjectClass base_class;
};


GType       gap_process_get_type    (void) G_GNUC_CONST;

GapProcess *gap_process_start       (const char     *cmd_line,
                                     GError        **error);

GapState    gap_process_get_state   (GapProcess     *proc);
void        gap_process_set_size    (GapProcess     *proc,
                                     int             width,
                                     int             height);


G_END_DECLS

#endif /* GAP_PROCESS_H */
