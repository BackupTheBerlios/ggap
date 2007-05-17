#############################################################################
##
#W  gap_types.py                  ggap package                 Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation. See COPYING file that comes with this
##  distribution.
##

import pango
import gobject
import gtk

_boxed_funcs = {}
def _register_boxed(typ, func):
    _boxed_funcs[typ] = func

_simple_attrs = {}
def _transform_boxed_simple(typ, obj):
    attrs = _simple_attrs[typ]
    ret = {}
    for a in attrs:
        ret[a] = getattr(obj, a)
    return ret
def _register_boxed_simple(typ, attrs):
    _simple_attrs[typ] = attrs
    _register_boxed(typ, _transform_boxed_simple)


def _transform_gdk_event(typ, event):
    dic = {'type': event.type}
    attrs = {
        gtk.gdk.MOTION_NOTIFY: ['time', 'x', 'y', 'state'],
        gtk.gdk.BUTTON_PRESS: ['time', 'x', 'y', 'button'],
        gtk.gdk._2BUTTON_PRESS: ['time', 'x', 'y', 'button'],
        gtk.gdk._3BUTTON_PRESS: ['time', 'x', 'y', 'button'],
        gtk.gdk.BUTTON_RELEASE: ['time', 'x', 'y', 'button'],
        gtk.gdk.SCROLL: ['time', 'x', 'y', 'state', 'direction'],
        gtk.gdk.KEY_PRESS: ['time', 'keyval', 'state'],
        gtk.gdk.KEY_RELEASE: ['time', 'keyval', 'state'],
        gtk.gdk.ENTER_NOTIFY: ['time', 'x', 'y', 'mode', 'detail', 'focus', 'state'],
        gtk.gdk.LEAVE_NOTIFY: ['time', 'x', 'y', 'mode', 'detail', 'focus', 'state'],
        gtk.gdk.FOCUS_CHANGE: ['in_'],
        gtk.gdk.CONFIGURE: ['x', 'y', 'width', 'height'],
    }
    for a in attrs.get(event.type, []):
        dic[a] = getattr(event, a)
    return dic
_register_boxed(gtk.gdk.Event, _transform_gdk_event)


_register_boxed_simple(pango.Color, ['red', 'green', 'blue'])
_register_boxed_simple(gtk.gdk.Color, ['red', 'green', 'blue'])
_register_boxed_simple(gtk.gdk.Rectangle, ['x', 'y', 'width', 'height'])
_register_boxed_simple(gtk.Requisition, ['width', 'height'])
_register_boxed_simple(gtk.TextAttributes, ["bg_color", "fg_color", "bg_stipple",
                                            "fg_stipple", "rise", "underline", "strikethrough",
                                            "draw_bg", "justification", "direction", "font", "font_scale",
                                            "left_margin", "indent", "right_margin", "pixels_above_lines",
                                            "pixels_below_lines", "pixels_inside_wrap", "tabs", "wrap_mode",
                                            "language", "invisible", "bg_full_height", "editable", "realized"])

def transform(obj):
    if isinstance(obj, gobject.GBoxed):
        typ = type(obj)
        func = _boxed_funcs.get(typ)
        if func:
            return func(typ, obj)
        else:
            return obj
    elif isinstance(obj, gobject.GEnum) or \
         isinstance(obj, gobject.GFlags):
        return int(obj)
    return obj

if __name__ == '__main__':
    for t in _boxed_funcs:
        print t, _boxed_funcs[t]
    print transform(gtk.gdk.Event(gtk.gdk.NOTHING))
    print transform(gtk.gdk.Color())
    print transform(gtk.gdk.Rectangle())
    print transform(gtk.VISIBLE)
    print transform(gtk.WINDOW_TOPLEVEL)
