/*
 *   moocanvasitem.h
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

#ifndef __MOO_CANVAS_ITEM_H__
#define __MOO_CANVAS_ITEM_H__

#include <gtk/gtkwidget.h>

G_BEGIN_DECLS


#define MOO_TYPE_CANVAS_ITEM             (moo_canvas_item_get_type ())
#define MOO_CANVAS_ITEM(object)          (G_TYPE_CHECK_INSTANCE_CAST ((object), MOO_TYPE_CANVAS_ITEM, MooCanvasItem))
#define MOO_CANVAS_ITEM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_CANVAS_ITEM, MooCanvasItemClass))
#define MOO_IS_CANVAS_ITEM(object)       (G_TYPE_CHECK_INSTANCE_TYPE ((object), MOO_TYPE_CANVAS_ITEM))
#define MOO_IS_CANVAS_ITEM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_CANVAS_ITEM))
#define MOO_CANVAS_ITEM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_CANVAS_ITEM, MooCanvasItemClass))

typedef enum {
    MOO_CANVAS_REALIZED = 1 << 0,
    MOO_CANVAS_VISIBLE  = 1 << 1
} MooCanvasItemFlags;

#define MOO_CANVAS_ITEM_GET_FLAGS(i,f)      (((MooCanvasItem*)i)->flags & (f))
#define MOO_CANVAS_ITEM_SET_FLAGS(i,f)      ((MooCanvasItem*)i)->flags |= (f)
#define MOO_CANVAS_ITEM_UNSET_FLAGS(i,f)    ((MooCanvasItem*)i)->flags &= ~(f)
#define MOO_CANVAS_ITEM_REALIZED(item)      MOO_CANVAS_ITEM_GET_FLAGS (item, MOO_CANVAS_REALIZED)
#define MOO_CANVAS_ITEM_VISIBLE(item)       MOO_CANVAS_ITEM_GET_FLAGS (item, MOO_CANVAS_VISIBLE)


typedef struct _MooCanvasRect MooCanvasRect;
typedef struct _MooCanvasStyle MooCanvasStyle;

struct _MooCanvasRect {
    double x1;
    double y1;
    double x2;
    double y2;
};

struct _MooCanvasStyle {
    int line_width;
    GdkColor fg;
    GdkColor bg;
    PangoFontDescription *font;
};


typedef struct _MooCanvasItem      MooCanvasItem;
typedef struct _MooCanvasItemClass MooCanvasItemClass;

struct _MooCanvasItem
{
    GtkObject base;

    GtkWidget *canvas;
    MooCanvasItemFlags flags;

    GdkGC *gc;
    MooCanvasStyle *style;
};

struct _MooCanvasItemClass
{
    GtkObjectClass base_class;

    void        (*realize)      (MooCanvasItem  *item);
    void        (*unrealize)    (MooCanvasItem  *item);

    GdkRegion*  (*get_region)   (MooCanvasItem  *item);
    void        (*move)         (MooCanvasItem  *item,
                                 double          dx,
                                 double          dy);

    gboolean    (*event)        (MooCanvasItem  *item,
                                 GdkEvent       *event);
    void        (*draw)         (MooCanvasItem  *item,
                                 GdkEventExpose *event,
                                 GdkDrawable    *dest);
};


GType       moo_canvas_item_get_type        (void) G_GNUC_CONST;

void        _moo_canvas_item_set_parent     (MooCanvasItem  *item,
                                             GtkWidget      *widget);
void        _moo_canvas_item_unparent       (MooCanvasItem  *item);

void        moo_canvas_item_realize         (MooCanvasItem  *item);
void        moo_canvas_item_unrealize       (MooCanvasItem  *item);

void        moo_canvas_item_move            (MooCanvasItem  *item,
                                             double          dx,
                                             double          dy);

gboolean    _moo_canvas_item_event          (MooCanvasItem  *item,
                                             GdkEvent       *event);
gboolean    _moo_canvas_item_draw           (MooCanvasItem  *item,
                                             GdkEventExpose *event,
                                             GdkDrawable    *dest);

void        moo_canvas_item_invalidate      (MooCanvasItem  *item);
GdkRegion  *moo_canvas_item_get_region      (MooCanvasItem  *item);

void        moo_canvas_widget_to_item       (MooCanvasItem  *item,
                                             int             wx,
                                             int             wy,
                                             double         *ix,
                                             double         *iy);
void        moo_canvas_item_to_widget       (MooCanvasItem  *item,
                                             double          ix,
                                             double          iy,
                                             int            *wx,
                                             int            *wy);
void        moo_canvas_rect_item_to_widget  (MooCanvasItem  *item,
                                             MooCanvasRect  *irect,
                                             GdkRectangle   *wrect);

MooCanvasStyle *moo_canvas_style_new        (void);
MooCanvasStyle *moo_canvas_style_copy       (MooCanvasStyle *style);
void        moo_canvas_style_free           (MooCanvasStyle *style);


G_END_DECLS

#endif /* __MOO_CANVAS_ITEM_H__ */
