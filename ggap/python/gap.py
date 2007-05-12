#############################################################################
##
#W  gap.py                     ggap package                    Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation. See COPYING file that comes with this
##  distribution.
##

import gtk
import pango
import moo

def GladeXML(filename, root=None):
    def signal_func(xml, widget_id, widget, signal, handler, object, siglist):
        siglist.append({'widget_id': widget_id, 'widget': widget,
                        'signal': signal, 'handler': handler, 'object': object})
        return True

    signals = []
    xml = moo.utils.GladeXML()
    xml.set_signal_func(signal_func, signals)
    xml.parse_file(filename, root)

    return {'xml': xml, 'signals': signals}


def Button(label_or_stock=None, use_underline=True):
    if label_or_stock is None:
        return gtk.Button()
    if gtk.stock_lookup(label_or_stock):
        return gtk.Button(stock=label_or_stock, use_underline=use_underline)
    else:
        return gtk.Button(label=label_or_stock, use_underline=use_underline)

def list_store_remove(store, pos):
    store.remove(store.get_iter((pos,)))
def list_store_insert(store, pos, item):
    store.insert(pos, item)
def list_store_append(store, item):
    store.append(item)
def list_store_set(store, path, item):
    store[path] = item
def list_store_get(store, path):
    return list(store[path])
def list_store_set_list(store, items):
    store.clear()
    for i in items:
        store.append(i)
def list_store_get_list(store):
    return [list(row) for row in store]


def tree_selection_get_selected(sel):
    ret = sel.get_selected()
    if ret[1] is not None:
        return (ret[0], ret[0].get_path(ret[1]))
    else:
        return ret


def widget_modify_font(widget, font):
    if font:
        font = pango.FontDescription(font)
    widget.modify_font(font)


def text_view_set_text(textview, text):
    textview.get_buffer().set_text(text)
def text_view_get_text(textview, text):
    buf = textview.get_buffer()
    return buf.get_text(buf.get_start_iter(), buf.get_end_iter())
