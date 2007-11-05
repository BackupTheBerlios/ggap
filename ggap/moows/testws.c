#include "moows/moowstextblock.h"
#include "moows/moowspromptblock.h"
#include <gtk/gtk.h>

static void
add_blocks (MooWsView *view)
{
    MooWsBlock *block;

    block = g_object_new (MOO_TYPE_WS_PROMPT_BLOCK, "ps", "gap> ",
                          "ps2", "> ", "text", "lalalala\nlalalala",
                          NULL);
    moo_ws_view_append_block (view, block);
    g_object_set (block->tag, "paragraph-background", "yellow", NULL);

    block = g_object_new (MOO_TYPE_WS_TEXT_BLOCK, NULL);
    moo_ws_view_append_block (view, block);
    moo_ws_text_block_set_text (MOO_WS_TEXT_BLOCK (block), "efwefwefwef");
    g_object_set (block->tag, "paragraph-background", "green", NULL);

    block = g_object_new (MOO_TYPE_WS_PROMPT_BLOCK, "ps", "gap> ",
                          "ps2", "> ", "text", "foobatr",
                          NULL);
    moo_ws_view_append_block (view, block);
    g_object_set (block->tag, "paragraph-background", "yellow", NULL);

    block = g_object_new (MOO_TYPE_WS_TEXT_BLOCK, NULL);
    moo_ws_view_append_block (view, block);
    moo_ws_text_block_set_text (MOO_WS_TEXT_BLOCK (block), "tyjtyjtyjtyj");
    g_object_set (block->tag, "paragraph-background", "blue", NULL);
}

int
main (int argc, char *argv[])
{
    MooWsView *view;
    GtkWidget *window;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), 500, 400);
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    view = g_object_new (MOO_TYPE_WS_VIEW, NULL);
    gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (view));

    add_blocks (view);

    gtk_widget_show_all (window);
    gtk_main ();

    return 0;
}
