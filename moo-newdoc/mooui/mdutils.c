/*
 *   mdutils.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "config.h"

#define GDK_PIXBUF_ENABLE_BACKEND
#include <gdk-pixbuf/gdk-pixbuf-animation.h>

#include "mooui/mdutils.h"
/* sys/stat.h macros */
#include "mooutils/mooutils-fs.h"
#include <glib/gstdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif


char *
_md_uri_get_dirname (const char *uri)
{
    const char *last_slash;

    g_return_val_if_fail (uri != NULL, NULL);

    if (!g_str_has_prefix (uri, "file://"))
        return NULL;

    last_slash = strrchr (uri, '/');
    if (last_slash && last_slash > uri && last_slash[-1] != '/')
        return g_strndup (uri, last_slash - uri);

    return NULL;
}


void
_md_widget_beep (GtkWidget *widget)
{
#if GTK_CHECK_VERSION(2,12,0)
    if (widget && GTK_WIDGET_REALIZED (widget))
        gdk_window_beep (GTK_WIDGET (widget)->window);
    else
#endif
    if (widget && GTK_WIDGET_REALIZED (widget))
        gdk_display_beep (gtk_widget_get_display (widget));
    else
        gdk_display_beep (gdk_display_get_default ());
}


#define MD_TYPE_THROBBER_ANIMATION              (md_throbber_animation_get_type ())
#define MD_THROBBER_ANIMATION(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_THROBBER_ANIMATION, MdThrobberAnimation))
#define MD_THROBBER_ANIMATION_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_THROBBER_ANIMATION, MdThrobberAnimationClass))
#define MD_IS_THROBBER_ANIMATION(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_THROBBER_ANIMATION))
#define MD_IS_THROBBER_ANIMATION_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_THROBBER_ANIMATION))
#define MD_THROBBER_ANIMATION_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_THROBBER_ANIMATION, MdThrobberAnimationClass))

#define MD_TYPE_THROBBER_ITER              (md_throbber_iter_get_type ())
#define MD_THROBBER_ITER(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_THROBBER_ITER, MdThrobberIter))
#define MD_THROBBER_ITER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_THROBBER_ITER, MdThrobberIterClass))
#define MD_IS_THROBBER_ITER(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_THROBBER_ITER))
#define MD_IS_THROBBER_ITER_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_THROBBER_ITER))
#define MD_THROBBER_ITER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_THROBBER_ITER, MdThrobberIterClass))

typedef struct {
    GdkPixbufAnimation base;
    int width;
    int height;
    GdkPixbuf **frames; /* includes zero-th frame, no idea why */
    guint n_frames;
} MdThrobberAnimation;

typedef struct {
    GdkPixbufAnimationClass base_class;
} MdThrobberAnimationClass;

typedef struct {
    GdkPixbufAnimationIter base;
    MdThrobberAnimation *anim;
    GTimeVal last_time;
    int delay_time; /* ms */
    int elapsed;    /* ms */
    guint current; /* anim->frames[current] is the current frame */
} MdThrobberIter;

typedef struct {
    GdkPixbufAnimationIterClass base_class;
} MdThrobberIterClass;

GType md_throbber_animation_get_type (void) G_GNUC_CONST;
GType md_throbber_iter_get_type (void) G_GNUC_CONST;

G_DEFINE_TYPE (MdThrobberAnimation, md_throbber_animation, GDK_TYPE_PIXBUF_ANIMATION)
G_DEFINE_TYPE (MdThrobberIter, md_throbber_iter, GDK_TYPE_PIXBUF_ANIMATION_ITER)

static void
md_throbber_animation_init (MdThrobberAnimation *anim)
{
    anim->width = 1;
    anim->height = 1;
    anim->frames = NULL;
    anim->n_frames = 0;
}

static void
md_throbber_animation_finalize (GObject *object)
{
    guint i;
    MdThrobberAnimation *anim = MD_THROBBER_ANIMATION (object);

    for (i = 0; i < anim->n_frames; ++i)
        g_object_unref (anim->frames[i]);
    g_free (anim->frames);

    G_OBJECT_CLASS (md_throbber_animation_parent_class)->finalize (object);
}

static gboolean
md_throbber_animation_is_static_image (GdkPixbufAnimation *anim)
{
    MdThrobberAnimation *thr = MD_THROBBER_ANIMATION (anim);
    return thr->n_frames == 1;
}

static GdkPixbuf *
md_throbber_animation_get_static_image (GdkPixbufAnimation *anim)
{
    MdThrobberAnimation *thr = MD_THROBBER_ANIMATION (anim);
    return thr->n_frames != 0 ? thr->frames[0] : NULL;
}

static void
md_throbber_animation_get_size (GdkPixbufAnimation *anim,
                                int                *width,
                                int                *height)
{
    MdThrobberAnimation *thr = MD_THROBBER_ANIMATION (anim);

    if (width)
        *width = thr->width;
    if (height)
        *height = thr->height;
}

static GdkPixbufAnimationIter *
md_throbber_animation_get_iter (GdkPixbufAnimation *anim,
                                const GTimeVal     *start_time)
{
    MdThrobberAnimation *thr = MD_THROBBER_ANIMATION (anim);
    MdThrobberIter *iter;

    g_return_val_if_fail (thr->n_frames > 1, NULL);

    iter = g_object_new (MD_TYPE_THROBBER_ITER, NULL);
    iter->anim = g_object_ref (thr);
    iter->current = 1;
    iter->last_time = *start_time;
    iter->delay_time = 1000/15; /* 15 fps */

    return GDK_PIXBUF_ANIMATION_ITER (iter);
}

static void
md_throbber_animation_class_init (MdThrobberAnimationClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GdkPixbufAnimationClass *anim_class = GDK_PIXBUF_ANIMATION_CLASS (klass);

    object_class->finalize = md_throbber_animation_finalize;

    anim_class->is_static_image = md_throbber_animation_is_static_image;
    anim_class->get_static_image = md_throbber_animation_get_static_image;
    anim_class->get_size = md_throbber_animation_get_size;
    anim_class->get_iter = md_throbber_animation_get_iter;
}



static void
md_throbber_iter_init (MdThrobberIter *iter)
{
    iter->anim = NULL;
}

static void
md_throbber_iter_finalize (GObject *object)
{
    MdThrobberIter *iter = MD_THROBBER_ITER (object);

    g_object_unref (iter->anim);

    G_OBJECT_CLASS (md_throbber_animation_parent_class)->finalize (object);
}

static int
md_throbber_iter_get_delay_time (GdkPixbufAnimationIter *iter)
{
    MdThrobberIter *thr_iter = MD_THROBBER_ITER (iter);
    return thr_iter->delay_time - thr_iter->elapsed;
}

static GdkPixbuf *
md_throbber_iter_get_pixbuf (GdkPixbufAnimationIter *iter)
{
    MdThrobberIter *thr_iter = MD_THROBBER_ITER (iter);
    return thr_iter->anim->frames[thr_iter->current];
}

static gboolean
md_throbber_iter_on_currently_loading_frame (G_GNUC_UNUSED GdkPixbufAnimationIter *iter)
{
    return FALSE;
}

static gboolean
md_throbber_iter_advance (GdkPixbufAnimationIter *iter,
                          const GTimeVal         *current_time)
{
    MdThrobberIter *thr_iter = MD_THROBBER_ITER (iter);
    int elapsed;
    int add_frames;

    elapsed = 1000*(current_time->tv_sec - thr_iter->last_time.tv_sec) +
              (current_time->tv_usec - thr_iter->last_time.tv_usec) / 1000;
    if (elapsed < 0)
        elapsed = 0;
    thr_iter->last_time = *current_time;

    if (thr_iter->elapsed + elapsed < thr_iter->delay_time)
    {
        thr_iter->elapsed += elapsed;
        return FALSE;
    }

    add_frames = (elapsed - thr_iter->delay_time + thr_iter->elapsed) / thr_iter->delay_time + 1;
    thr_iter->elapsed = (elapsed - thr_iter->delay_time + thr_iter->elapsed) % thr_iter->delay_time;
    /* skip zero-th frame */
    thr_iter->current = 1 + (thr_iter->current - 1 + add_frames) % (thr_iter->anim->n_frames - 1);

    return TRUE;
}

static void
md_throbber_iter_class_init (MdThrobberIterClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GdkPixbufAnimationIterClass *iter_class = GDK_PIXBUF_ANIMATION_ITER_CLASS (klass);

    object_class->finalize = md_throbber_iter_finalize;

    iter_class->get_delay_time = md_throbber_iter_get_delay_time;
    iter_class->get_pixbuf = md_throbber_iter_get_pixbuf;
    iter_class->on_currently_loading_frame = md_throbber_iter_on_currently_loading_frame;
    iter_class->advance = md_throbber_iter_advance;
}


static MdThrobberAnimation *
get_throbber_for_icon_size (GtkIconTheme *icon_theme,
                            GtkIconSize   icon_size,
                            GtkSettings  *settings)
{
    GtkIconInfo *icon_info;
    int size, height_dummy;
    const char *filename;
    GdkPixbuf *base_pixbuf;
    MdThrobberAnimation *anim;
    guint i, j;
    guint n_horiz, n_vert;

    gtk_icon_size_lookup_for_settings (settings, icon_size, &size, &height_dummy);
    icon_info = gtk_icon_theme_lookup_icon (icon_theme, "process-working", size, 0);

    if (!icon_info)
        return NULL;

    filename = gtk_icon_info_get_filename (icon_info);
    size = gtk_icon_info_get_base_size (icon_info);

    if (!filename || size <= 0)
    {
        gtk_icon_info_free (icon_info);
        return NULL;
    }

    base_pixbuf = gdk_pixbuf_new_from_file (filename, NULL);

    if (!base_pixbuf)
    {
        gtk_icon_info_free (icon_info);
        return NULL;
    }

    gtk_icon_info_free (icon_info);

    if (gdk_pixbuf_get_width (base_pixbuf) % size != 0 ||
        gdk_pixbuf_get_height (base_pixbuf) % size != 0)
    {
        g_object_unref (base_pixbuf);
        return NULL;
    }

    if (gdk_pixbuf_get_width (base_pixbuf) / size <= 1 &&
        gdk_pixbuf_get_height (base_pixbuf) / size <= 1)
    {
        g_object_unref (base_pixbuf);
        return NULL;
    }

    n_horiz = gdk_pixbuf_get_width (base_pixbuf) / size;
    n_vert = gdk_pixbuf_get_height (base_pixbuf) / size;

    anim = g_object_new (MD_TYPE_THROBBER_ANIMATION, NULL);
    anim->width = size;
    anim->height = size;
    anim->n_frames = n_horiz * n_vert;
    g_assert (anim->n_frames != 0);
    anim->frames = g_new0 (GdkPixbuf*, anim->n_frames);

    for (i = 0; i < n_vert; ++i)
        for (j = 0; j < n_horiz; ++j)
            anim->frames[i*n_horiz + j] =
                gdk_pixbuf_new_subpixbuf (base_pixbuf, j*size, i*size, size, size);

    g_object_unref (base_pixbuf);
    return anim;
}

static void
icon_theme_changed (GtkIconTheme *icon_theme)
{
    g_object_set_data (G_OBJECT (icon_theme), "moo-throbber-cache", NULL);
    g_signal_handlers_disconnect_by_func (icon_theme, (gpointer) icon_theme_changed, NULL);
}

static MdThrobberAnimation *
get_throbber (GtkImage    *image,
              GtkIconSize  size)
{
    GdkScreen *screen;
    GHashTable *hash;
    GtkIconTheme *icon_theme;
    gpointer dummy, pthrobber;

    if (gtk_widget_has_screen (GTK_WIDGET (image)))
        screen = gtk_widget_get_screen (GTK_WIDGET (image));
    else
        screen = gdk_screen_get_default ();

    icon_theme = gtk_icon_theme_get_for_screen (screen);
    hash = g_object_get_data (G_OBJECT (icon_theme), "moo-throbber-cache");

    if (!hash)
    {
        hash = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_object_unref);
        g_object_set_data_full (G_OBJECT (icon_theme), "moo-throbber-cache",
                                hash, (GDestroyNotify) g_hash_table_destroy);
        g_signal_connect (icon_theme, "changed", G_CALLBACK (icon_theme_changed), NULL);
    }

    if (!g_hash_table_lookup_extended (hash, GINT_TO_POINTER (size), &dummy, &pthrobber))
    {
        MdThrobberAnimation *throbber;
        throbber = get_throbber_for_icon_size (icon_theme, size,
                                               gtk_widget_get_settings (GTK_WIDGET (image)));
        g_assert (!throbber || GDK_IS_PIXBUF_ANIMATION (throbber));
        g_hash_table_insert (hash, GINT_TO_POINTER (size), throbber);
        pthrobber = throbber;
    }

    g_assert (!pthrobber || GDK_IS_PIXBUF_ANIMATION (pthrobber));
    return pthrobber;
}

gboolean
_md_image_attach_throbber (GtkImage    *image,
                           GtkIconSize  size)
{
    MdThrobberAnimation *throbber;

    g_return_val_if_fail (GTK_IS_IMAGE (image), FALSE);

    if (!(throbber = get_throbber (image, size)))
        return FALSE;

    g_assert (!throbber || GDK_IS_PIXBUF_ANIMATION (throbber));
    gtk_image_set_from_animation (image, GDK_PIXBUF_ANIMATION (throbber));

    return TRUE;
}
