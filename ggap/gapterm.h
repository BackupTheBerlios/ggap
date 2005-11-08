/*
 *   gapterm.h
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

#ifndef __GAP_TERM_H__
#define __GAP_TERM_H__

#include <mooterm/mooterm.h>

G_BEGIN_DECLS


#define GAP_TYPE_TERM             (gap_term_get_type ())
#define GAP_TERM(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_TERM, GapTerm))
#define GAP_TERM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_TERM, GapTermClass))
#define GAP_IS_TERM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_TERM))
#define GAP_IS_TERM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_TERM))
#define GAP_TERM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_TERM, GapTermClass))


typedef struct _GapTerm        GapTerm;
typedef struct _GapTermPrivate GapTermPrivate;
typedef struct _GapTermClass   GapTermClass;


struct _GapTerm
{
    MooTerm base;
    GapTermPrivate *priv;
};

struct _GapTermClass
{
    MooTermClass base_class;
};


GType   gap_term_get_type   (void) G_GNUC_CONST;


G_END_DECLS

#endif /* __GAP_TERM_H__ */
