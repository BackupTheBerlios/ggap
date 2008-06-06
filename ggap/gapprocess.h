/*
 *   gapprocess.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
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

    void (*gap_died)            (GapProcess *proc);
    void (*gap_output)          (GapProcess *proc,
                                 const char *data,
                                 guint       data_len,
                                 gboolean    is_stderr);
    void (*gap_prompt)          (GapProcess *proc,
                                 const char *string,
                                 guint       len,
                                 gboolean    first_time);
    void (*gap_globals_changed) (GapProcess *proc,
                                 const char *data,
                                 guint       data_len,
                                 gboolean    added);
};


GType       gap_process_get_type        (void) G_GNUC_CONST;

GapProcess *gap_process_start           (const char     *cmd_line,
                                         int             width,
                                         int             height,
                                         GError        **error);

GapState    gap_process_get_state       (GapProcess     *proc);
void        gap_process_set_size        (GapProcess     *proc,
                                         int             width,
                                         int             height);

void        gap_process_write_input     (GapProcess     *proc,
                                         char          **lines);
gboolean    gap_process_run_command     (GapProcess     *proc,
                                         const char     *command,
                                         const char     *args,
                                         const char     *gap_cmd_line,
                                         char          **output);
void        gap_process_ask_completions (GapProcess     *proc);

void        gap_process_die             (GapProcess     *proc);


G_END_DECLS

#endif /* GAP_PROCESS_H */
