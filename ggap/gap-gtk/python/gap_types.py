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
import math


def split_float(val):
    sign = 1
    if val < 0:
        sign = -1
        val = -val
    m, denom = math.frexp(val)
    if m == 0.0:
        return 0, 1
    numer = 0
    while m != 0.0:
        m *= 2
        numer = 2*numer + int(m)
        denom -= 1
        m = m - int(m)

    numer *= sign
    if denom >= 0:
        return numer * 2**denom, 1
    else:
        return numer, 2**(-denom)


#############################################################################
##
##  Boxed types
##

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


#############################################################################
##
##  Properties
##

_prop_setters = {}
def _register_prop_setter(obj_type, propname, func):
    allnames = [propname.replace('-', '_'), propname.replace('_', '-')]
    if allnames[1] == allnames[0]:
        allnames = allnames[:1]
    for name in allnames:
        funcs = _prop_setters.get(name)
        if funcs:
            funcs.append((obj_type, func))
        else:
            _prop_setters[name] = [(obj_type, func)]
def _register_prop_setter_tr(obj_type, propname, func):
    def set_prop(obj, name, value):
        obj.set_property(name, func(value))
    _register_prop_setter(obj_type, propname, set_prop)

def set_property(obj, propname, value):
    if isinstance(propname, dict):
        for p in propname:
            set_property(obj, p, prop[p])
        return
    funcs = _prop_setters.get(propname)
    if funcs:
        for typ, func in funcs:
            if isinstance(obj, typ):
                func(obj, propname, value)
                return
    obj.set_property(prop, value)


_prop_getters = {}
def _register_prop_getter(obj_type, propname, func):
    for name in [propname.replace('-', '_'), propname.replace('_', '-')]:
        funcs = _prop_getters.get(name)
        if funcs:
            funcs.append((obj_type, func))
        else:
            _prop_getters[name] = [(obj_type, func)]
def _register_prop_getter_tr(obj_type, propname, func):
    def get_prop(obj, name, value):
        return func(obj.get_property(name))
    _register_prop_getter(obj_type, propname, get_prop)

def get_property(obj, propname):
    funcs = _prop_getters.get(propname)
    if funcs:
        for typ, func in funcs:
            if isinstance(obj, typ):
                return func(obj, propname)
    return obj.get_property(prop)


_register_prop_setter_tr(gtk.ComboBox, 'active', lambda n: n-1)
_register_prop_getter_tr(gtk.ComboBox, 'active', lambda n: n+1)


if __name__ == '__main__':
    print "=== boxed funcs ==="
    for t in _boxed_funcs:
        print t, _boxed_funcs[t]
    print
    print "=== boxed ==="
    print transform(gtk.gdk.Event(gtk.gdk.NOTHING))
    print transform(gtk.gdk.Color())
    print transform(gtk.gdk.Rectangle())
    print
    print "=== enums ==="
    print transform(gtk.VISIBLE)
    print transform(gtk.WINDOW_TOPLEVEL)
    print
    print "=== floats ==="
    def print_fl(val):
        n,d = split_float(val)
        print '%s == %s' % (val, float(n)/d)
    print_fl(0.0)
    print_fl(0.1)
    print_fl(0.3333333333333333333)
    print_fl(0.5)
    print_fl(8.)
    print_fl(-0.1)
    print_fl(-11234235345.3463645645)
