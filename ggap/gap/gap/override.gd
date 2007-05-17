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
#O  GladeXML( <filename> [, <root>] [, <callbacks>[, <types>]] )
##
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


DeclareGlobalFunction("GTK_TREE_PATH_G2P");
DeclareGlobalFunction("GTK_TREE_PATH_P2G");
DeclareGlobalFunction("GTK_TREE_INDEX_G2P");
