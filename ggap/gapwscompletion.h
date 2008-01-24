/*
 *   gapwscompletion.h
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

#ifndef GAP_WS_COMPLETION_H
#define GAP_WS_COMPLETION_H

#include "mooedit/moocompletionsimple.h"

G_BEGIN_DECLS


#define GAP_TYPE_WS_COMPLETION             (gap_ws_completion_get_type ())
#define GAP_WS_COMPLETION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_WS_COMPLETION, GapWsCompletion))
#define GAP_WS_COMPLETION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_WS_COMPLETION, GapWsCompletionClass))
#define GAP_IS_WS_COMPLETION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_WS_COMPLETION))
#define GAP_IS_WS_COMPLETION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_WS_COMPLETION))
#define GAP_WS_COMPLETION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_WS_COMPLETION, GapWsCompletionClass))

typedef struct _GapWsCompletion        GapWsCompletion;
typedef struct _GapWsCompletionClass   GapWsCompletionClass;

struct _GapWsCompletion
{
    MooCompletionSimple base;
};

struct _GapWsCompletionClass
{
    MooCompletionSimpleClass base_class;
};


GType               gap_ws_completion_get_type      (void) G_GNUC_CONST;

MooTextCompletion  *gap_ws_completion_new           (void);

void                gap_ws_completion_add           (const char *data,
                                                     gsize       data_len);
void                gap_ws_completion_remove        (const char *data,
                                                     gsize       data_len);


G_END_DECLS

#endif /* GAP_WS_COMPLETION_H */
