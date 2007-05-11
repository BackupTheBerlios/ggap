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

import gobject
import gtk

_boxed_funcs = {}
def _register_boxed(typ, func):
    _boxed_funcs[typ] = func


#############################################################################
##
##  gtk.gdk.Event
##
def _transform_gdk_event(event):
    dic = {'type': event.type}
    attrs = {
        gtk.gdk.MOTION_NOTIFY: ['time', 'x', 'y', 'state'],
        gtk.gdk.BUTTON_PRESS: ['time', 'x', 'y', 'button'],
        gtk.gdk._2BUTTON_PRESS: ['time', 'x', 'y', 'button'],
        gtk.gdk._3BUTTON_PRESS: ['time', 'x', 'y', 'button'],
        gtk.gdk.BUTTON_RELEASE: ['time', 'x', 'y', 'button'],
        gtk.gdk.gtk.gdk.SCROLL: ['time', 'x', 'y', 'state', 'direction'],
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


def transform(obj):
    if isinstance(obj, gobject.GBoxed):
        func = _boxed_funcs.get(type(obj))
        if func:
            return func(obj)
        else:
            return obj
    return obj

if __name__ == '__main__':
    for t in _boxed_funcs:
        print t, _boxed_funcs[t]
