#############################################################################
##
#W  gap_cb.py                    ggap package                  Yevgen Muntyan
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

_transform_funcs = {}

def _register(func, typ, signal):
    funcs = _transform_funcs.get(signal)
    if funcs:
        funcs.append([typ, func])
    else:
        funcs = [[typ, func]]
        _transform_funcs[signal] = funcs
        norm_sig = signal.replace('_', '-')
        if norm_sig != signal:
            _transform_funcs[norm_sig] = funcs


#############################################################################
##
## gtk.Widget
##
def _transform_delete_event(wid, event):
    return [wid]
_register(_transform_delete_event, gtk.Widget, 'delete_event')


#############################################################################
##
## gtk.TreeView
##

def _tree_path_p2g(path):
    if path is None:
        return None
    elif len(path) == 1:
        return path[0] + 1
    else:
        return map(lambda i: i+1, path)

def _transform_treeview_path_column(treeview, path, view_column):
    return [treeview, _tree_path_p2g(path), view_column]
_register(_transform_treeview_path_column, gtk.TreeView, 'row_activated')

def _transform_treeview_iter_path(treeview, iter, path):
    return [treeview, _tree_path_p2g(path)]
_register(_transform_treeview_iter_path, gtk.TreeView, 'row_collapsed')
_register(_transform_treeview_iter_path, gtk.TreeView, 'row_expanded')
_register(_transform_treeview_iter_path, gtk.TreeView, 'test_collapse_row')
_register(_transform_treeview_iter_path, gtk.TreeView, 'test_expand_row')


#############################################################################
##
## gtk.TreeModel
##

def _transform_model_path_iter(model, path, iter):
    return [model, _tree_path_p2g(path)]
_register(_transform_model_path_iter, gtk.TreeModel, 'row_changed')
_register(_transform_model_path_iter, gtk.TreeModel, 'row_has_child_toggled')
_register(_transform_model_path_iter, gtk.TreeModel, 'row_inserted')

def _transform_model_path(model, path):
    return [model, _tree_path_p2g(path)]
_register(_transform_model_path, gtk.TreeModel, 'row_deleted')


def _get_transform_func(obj, signal):
    funcs = _transform_funcs.get(signal)
    if not funcs:
        return None
    for f in funcs:
        if isinstance(obj, f[0]):
            return f[1]
    return None

class Callback:
    def __init__(self, obj, signal, func):
        id = obj.connect(signal, self.callback, func)
        self.obj = obj
        self.id = id
        self.__transform_func = _get_transform_func(obj, signal)

    def callback(self, obj, *args):
        func = args[-1]
        args = self.__transform_args(args[:-1])
        return func(obj, self.id, args)

    def disconnect(self):
        self.obj.disconnect(self.id)
        self.obj = None

    def __transform_args(self, args):
        if self.__transform_func:
            return self.__transform_func(*args)
        else:
            return args

if __name__ == '__main__':
    for f in _transform_funcs:
        print f, _transform_funcs[f]
