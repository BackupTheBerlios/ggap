#############################################################################
##
#W  override.gd                  ggap package                  Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation. See COPYING file that comes with this
##  distribution.
##


###############################################################################
##
#O  HideOnDelete( <window> )
##
DeclareOperation("HideOnDelete", [IsGtkWidget]);


###############################################################################
##
#O  GladeXML( <filename>[, <callbacks>] )
##
DeclareOperation("GladeXML", [IsString, IsRecord]);
DeclareOperation("GladeXML", [IsString]);


###############################################################################
##
#O  GtkButton( [<label_or_stock>[, <use_underline>]] )
##
_GDeclareOperationOptArgs("GtkButton", [], [IsString, IsBool]);


###############################################################################
##
#A  NColumns
##
DeclareAttribute("NColumns", IsGtkTreeModel);

###############################################################################
##
#O  Clear( <list> )
##
DeclareOperation("Clear", [IsGtkTreeModel]);

###############################################################################
##
#O  Insert( <list>, <pos>, <item> )
#O  Append( <list>, <item> )
#O  Remove( <list>, <pos> )
##
DeclareOperation("Insert", [IsGtkTreeModel, IsList, IsObject]);
DeclareOperation("Insert", [IsGtkTreeModel, IsInt, IsObject]);
DeclareOperation("Append", [IsGtkTreeModel, IsObject]);
DeclareOperation("Remove", [IsGtkTreeModel, IsList]);
DeclareOperation("Remove", [IsGtkTreeModel, IsPosInt]);

###############################################################################
##
#O  SetItem( <list>, <pos>, <item> )
#O  GetItem( <list>, <pos> )
##
DeclareOperation("SetItem", [IsGtkTreeModel, IsPosInt, IsObject]);
DeclareOperation("SetItem", [IsGtkTreeModel, IsList, IsObject]);
DeclareOperation("GetItem", [IsGtkTreeModel, IsPosInt]);
DeclareOperation("GetItem", [IsGtkTreeModel, IsList]);

###############################################################################
##
#F  GtkListStore()
##
DeclareGlobalFunction("GtkListStore");

###############################################################################
##
#O  SetList( <listobj>, <items> )
#O  GetList( <listobj> )
##
DeclareOperation("SetList", [IsGtkListStore, IsList]);
DeclareOperation("GetList", [IsGtkListStore]);



###############################################################################
##
#O  ScrollToCell( <treeview>, <path>, <column=None>,
##                <use_align=FALSE>, <row_align=0.0>, <col_align=0.0> )
##
_GDeclareOperationOptArgs("ScrollToCell", [IsGtkTreeView, IsObject],
                          [IsGtkTreeViewColumn, IsBool, IsRat, IsRat]);

###############################################################################
##
#O  SetCursor( <treeview>, <path>, <focus_column=None>, <start_editing=False> )
##
_GDeclareOperationOptArgs("SetCursor", [IsGtkTreeView, IsObject],
                          [IsGtkTreeViewColumn, IsBool]);

###############################################################################
##
#O  SetCursorOnCell( <treeview>, <path>, <focus_column=None>, <focus_cell=None>,
##                   <start_editing=False> )
##
_GDeclareOperationOptArgs("SetCursorOnCell", [IsGtkTreeView, IsObject],
                          [IsGtkTreeViewColumn, IsGtkCellRenderer, IsBool]);

###############################################################################
##
#O  GetCursor( <treeview> )
##
DeclareOperation("GetCursor", [IsGtkTreeView]);

###############################################################################
##
#O  GetSelected( <tree_view> )
#O  GetSelectedRows( <tree_view> )
#O  SelectRow( <tree_view>, <path> )
#O  UnselectRow( <tree_view>, <path> )
#O  RowIsSelected( <tree_view>, <path> )
#O  SelectRange( <tree_view>, <start>, <end> )
##
DeclareOperation("GetSelected", [IsGtkTreeView]);
DeclareOperation("GetSelectedRows", [IsGtkTreeView]);
DeclareOperation("SelectRow", [IsGtkTreeView, IsObject]);
DeclareOperation("UnselectRow", [IsGtkTreeView, IsObject]);
DeclareOperation("RowIsSelected", [IsGtkTreeView, IsObject]);
DeclareOperation("SelectRange", [IsGtkTreeView, IsObject, IsObject]);


###############################################################################
##
#O  GetSelected( <tree_selection> )
#O  GetSelectedRows( <tree_selection> )
#O  SelectRow( <tree_selection>, <path> )
#O  UnselectRow( <tree_selection>, <path> )
#O  RowIsSelected( <tree_selection>, <path> )
#O  SelectRange( <tree_selection>, <start>, <end> )
##
DeclareOperation("GetSelected", [IsGtkTreeSelection]);
DeclareOperation("GetSelectedRows", [IsGtkTreeSelection]);
DeclareOperation("SelectRow", [IsGtkTreeSelection, IsObject]);
DeclareOperation("UnselectRow", [IsGtkTreeSelection, IsObject]);
DeclareOperation("RowIsSelected", [IsGtkTreeSelection, IsObject]);
DeclareOperation("SelectRange", [IsGtkTreeSelection, IsObject, IsObject]);
