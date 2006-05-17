/*
 *   moocanvastext.h
 *
 *   Copyright (C) 2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef __MOO_CANVAS_TEXT_H__
#define __MOO_CANVAS_TEXT_H__

#include <moocanvas/moocanvas.h>

G_BEGIN_DECLS


#define MOO_TYPE_CANVAS_TEXT             (moo_canvas_text_get_type ())
#define MOO_CANVAS_TEXT(object)          (G_TYPE_CHECK_INSTANCE_CAST ((object), MOO_TYPE_CANVAS_TEXT, MooCanvasText))
#define MOO_CANVAS_TEXT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_CANVAS_TEXT, MooCanvasTextClass))
#define MOO_IS_CANVAS_TEXT(object)       (G_TYPE_CHECK_INSTANCE_TYPE ((object), MOO_TYPE_CANVAS_TEXT))
#define MOO_IS_CANVAS_TEXT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_CANVAS_TEXT))
#define MOO_CANVAS_TEXT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_CANVAS_TEXT, MooCanvasTextClass))


typedef struct _MooCanvasText         MooCanvasText;
typedef struct _MooCanvasTextClass    MooCanvasTextClass;

struct _MooCanvasText
{
    MooCanvasItem base;

    char *text;
    PangoLayout *layout;
    double x, y;
};

struct _MooCanvasTextClass
{
    MooCanvasItemClass base_class;
};


GType   moo_canvas_text_get_type    (void) G_GNUC_CONST;


G_END_DECLS

#endif /* __MOO_CANVAS_TEXT_H__ */
