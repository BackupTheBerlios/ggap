#!/usr/bin/env python

import os
import sys
import gtk
import gobject
import gtktypes

try:
  import psyco
  psyco.full()
except ImportError:
  pass

pkg_name = 'ggap package'
author = 'Yevgen Muntyan'
header = """\
#############################################################################
##
#W  %s
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation. See COPYING file that comes with this
##  distribution.
##

##
##  THIS FILE IS AUTOGENERATED
##
"""

def printcats(cls, fp):
    if cls.name not in ["GObject"]:
        fp.write_gd(cls.doc)
        if cls.parents:
            fp.write_gd('DeclareCategory("%s", %s);' % (cls.gap_name,
                                                        ' and '.join([p.gap_name for p in cls.parents])))
        else:
            fp.write_gd('DeclareCategory("%s", %s);' % (cls.gap_name, 'IsObject'))
        fp.write_gd('')

    for child in cls.children:
        printcats(child, fp)

def printfunc(func, fp):
    fp.write_gd(func.get_doc(), name=func.gd_file)
    func.declare(fp)
    fp.write_gd('', name=func.gd_file)

def printops(cls, fp):
    for f in cls.methods:
        printfunc(f[1], fp)

    for child in cls.children:
        printops(child, fp)

def printmeths(cls, fp):
    for f in cls.methods:
        f[1].install(fp)

    for child in cls.children:
        printmeths(child, fp)

def printfuncdef(func, fp):
    func.install(fp)

def printtypes(cls, fp):
    if cls.name not in ["GObject"]:
        fp.write_gi('_GGAP_REGISTER_TYPE("%s", %s);' % (cls.name, cls.gap_name))

    for child in cls.children:
        printtypes(child, fp)

def printenums(mod, prefix, fp):
    ignoreenums = [gtk.PrivateFlags, gtk.ArgFlags, gtk.ButtonAction, gtk.CListDragPos, gtk.CellType,
                   gtk.CListDragPos, gtk.CTreeExpanderStyle, gtk.CTreeExpansionType, gtk.CTreeLineStyle,
                   gtk.CTreePos, gtk.DebugFlag, gtk.SubmenuDirection, gtk.SubmenuPlacement, gtk.MatchType,
                   gtk.PreviewType, gtk.SideType, gtk.PathPriorityType, gtk.AccelFlags, gtk.CellRendererState,
                   gtk.CurveType, gtk.DeleteType, gtk.DestDefaults, gtk.IMPreeditStyle, gtk.IMStatusStyle,
                   gtk.IconLookupFlags, gtk.IconThemeError, gtk.IconViewDropPosition, gtk.ImageType,
                   gtk.MenuDirectionType, gtk.MovementStep, gtk.NotebookTab, gtk.ObjectFlags, gtk.PathType,
                   gtk.RcFlags, gtk.RcTokenType, gtk.ResizeMode, gtk.StateType, gtk.TargetFlags, gtk.TextDirection,
                   gtk.TextWindowType, gtk.ToolbarChildType, gtk.ToolbarSpaceStyle, gtk.TreeModelFlags,
                   gtk.TreeViewDropPosition, gtk.UIManagerItemType, gtk.Visibility, gtk.WidgetHelpType]

    vals = []

    for w in dir(mod):
        m = getattr(mod, w)
        if (isinstance(m, gobject.GEnum) or isinstance(m, gobject.GFlags)) and \
            not type(m) in ignoreenums:
            vals.append([w, m])

    def cmp_vals(wm1, wm2):
        t1 = type(wm1[1])
        t2 = type(wm2[1])
        if t1 is t2:
            return cmp(wm1[1], wm2[1])
        else:
            return cmp(t1.__name__, t2.__name__)
    vals.sort(cmp_vals)

    def add_prefix(prefix, name):
        if name.startswith('_'):
            return prefix + name
        else:
            return prefix + '_' + name

    for wm in vals:
        fp.write_gi('BindGlobal("%s", %s);' % (add_prefix(prefix, wm[0]), int(wm[1])))

def printconstants(fp):
    for c in gtktypes.constants:
        c.install(fp)

def write_gd(fp):
    for cls in gtktypes.top_classes:
        printcats(cls, fp)
    fp.write_gd('')
    for cls in gtktypes.top_classes:
        printops(cls, fp)
    for func in gtktypes.functions:
        printfunc(func, fp)
    fp.write_gd('')
    fp.write_gd('DeclareGlobalFunction("_GGAP_REGISTER_WIDGETS");')

def write_gi(fp):
    printenums(gtk, "GTK", fp)
    printenums(gtk.gdk, "GDK", fp)
    printconstants(fp)
    fp.write_gi('')
    for cls in gtktypes.top_classes:
        printmeths(cls, fp)
    for func in gtktypes.functions:
        printfuncdef(func, fp)
    fp.write_gi('')
    fp.write_gi("InstallGlobalFunction(_GGAP_REGISTER_WIDGETS,")
    fp.write_gi("function()")
    for cls in gtktypes.top_classes:
        printtypes(cls, fp)
    fp.write_gi("end);")

class FileWriter(object):
    def __init__(self, gap_dir):
        self.gap_dir = gap_dir
        self.gi = self.__open("types.gi")
        self.files = {'types': self.__open("types.gd")}

    def write_gi(self, line):
        print >> self.gi, line

    def write_gd(self, line, name=None):
        if not name:
            name = 'types'
        f = self.files.get(name)
        if f is None:
            f = self.__open(name + '.gd')
            self.files[name] = f
        print >> f, line

    def __open(self, name):
        f = open(os.path.join(self.gap_dir, name), "w")
        spaces = (73 - len(name)- len(pkg_name) - len(author))/2
        spaces2 = 73 - len(name)- len(pkg_name) - len(author) - spaces
        c = '%s%s%s%s%s' % (name, ' '*spaces, pkg_name, ' '*spaces2, author)
        print >> f, header % (c,)
        return f

    def close(self):
        self.gi.close()
        for f in self.files:
            self.files[f].close()

if __name__ == '__main__':
    writer = FileWriter(sys.argv[1])
    write_gd(writer)
    write_gi(writer)
    writer.close()
