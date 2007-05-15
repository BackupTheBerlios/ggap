#############################################################################
##
#W  override.gi                  ggap package                  Yevgen Muntyan
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
InstallMethod(HideOnDelete, [IsGtkWidget],
function(w)
  Hide(w);
  return true;
end);


###############################################################################
##
##  GladeXML
##

###############################################################################
##
##  GladeXML( <filename>, <callbacks> )
##
InstallMethod(GladeXML, [IsString, IsRecord],
function(filename, callbacks)
  local ret, sig, args, data;

  ret := _GGAP_CALL_FUNC("gap.GladeXML", filename);

  for sig in ret.signals do
    args := [sig.widget, sig.signal];
    data := callbacks.(sig.handler);
    if IsFunction(data) then
      Add(args, data);
    else
      args := Concatenation(args, data);
    fi;
    CallFuncList(ConnectCallback, args);
  od;

  return ret.xml;
end);

###############################################################################
##
##  GladeXML( <filename>, <callbacks> )
##
InstallMethod(GladeXML, [IsString],
function(filename)
  return GladeXML(filename, rec());
end);

###############################################################################
##
##  xml.widget_name
##
InstallMethod(\., [IsGladeXML, IsPosInt],
function(xml, rnam)
  return GetWidget(xml, NameRNam(rnam));
end);

###############################################################################
##
##  IsBound(xml.widget_name)
##
InstallMethod(IsBound\., [IsGladeXML, IsPosInt],
function(xml, rnam)
  return GetWidget(xml, NameRNam(rnam)) <> GNone;
end);



###############################################################################
##
##  GtkButton( [<label_or_stock>[, <use_underline>]] )
##
_GInstallMethodsOptArgs(GtkButton, [], [GNone, true], false);

InstallMethod(GtkButton, [IsString, IsBool],
function(label_or_stock, underline)
  return _GGAP_CALL_FUNC("gap.Button", label_or_stock, underline);
end);

InstallMethod(GtkButton, [IsGNone, IsBool],
function(label_or_stock, underline)
  return _GGAP_CALL_FUNC("gap.Button", label_or_stock, underline);
end);



###############################################################################
##
##  GtkListStore
##

###############################################################################
##
##  GtkListStore()
##
InstallGlobalFunction(GtkListStore,
function(arg)
  local py_args, a, store;

  py_args := [];

  for a in arg do
    if a = IsInt then
      Add(py_args, "int");
    elif a = IsString then
      Add(py_args, "str");
    else
      Error("invalid argument ", a);
    fi;
  od;

  if Length(py_args) = 0 then
    Error("no arguments");
  fi;

  store := _GGAP_WRITE_FUNC("gtk.ListStore", py_args, "");
  SetNColumns(store, Length(py_args));

  return store;
end);

InstallMethod(NColumns, [IsGtkTreeModel],
function(store)
  return _GGAP_CALL_METH(store, "get_n_columns");
end);

###############################################################################
##
#M  Length( <list> )
##
InstallOtherMethod(Length, [IsGtkTreeModel],
function(store)
  return _GGAP_CALL_FUNC("len", store);
end);

BindGlobal("$GtkTreePathG2P", p -> List(p, i->i-1));
BindGlobal("$GtkTreePathP2G", p -> List(p, i->i+1));
BindGlobal("$GtkTreeIndexG2P", i -> i-1);
BindGlobal("$GtkTreeIndexP2G", i -> i+1);
BindGlobal("$GtkTreeItemG2P", function(item)
  if not IsList(item) or IsString(item) then
    return [item];
  else
    return item;
  fi;
end);
BindGlobal("$GtkTreeItemP2G", function(item)
  if Length(item) = 1 then
    return item[1];
  else
    return item;
  fi;
end);
BindGlobal("$GtkTreePosG2P",
function(p)
  if IsInt(p) then
    p := [p];
  fi;
  return $GtkTreePathG2P(p);
end);
BindGlobal("$GtkTreePosP2G",
function(p)
  if p = GNone then
    return 0;
  elif IsList(p) then
    if Length(p) = 1 then
      return p[1] + 1;
    else
      return List(p, i->i+1);
    fi;
  else
    return p + 1;
  fi;
end);

###############################################################################
##
##  Clear( <list> )
##
InstallMethod(Clear, [IsGtkListStore],
function(store)
  _GGAP_CALL_METH(store, "clear");
end);

###############################################################################
##
##  Insert( <list>, <pos>, <item> )
##
InstallMethod(Insert, [IsGtkListStore, IsList, IsList],
function(store, path, value)
  _GGAP_CALL_FUNC("gap.list_store_insert",
                  store,
                  $GtkTreeIndexG2P(path[1]),
                  $GtkTreeItemG2P(value));
end);

InstallMethod(Insert, [IsGtkListStore, IsInt, IsObject],
function(store, pos, value)
  Insert(store, [pos], value);
end);

###############################################################################
##
##  Append( <list>, <item> )
##
InstallMethod(Append, [IsGtkListStore, IsObject],
function(store, value)
  _GGAP_CALL_FUNC("gap.list_store_append", store, $GtkTreeItemG2P(value));
end);

###############################################################################
##
##  Remove( <list>, <pos> )
##
InstallMethod(Remove, [IsGtkListStore, IsList],
function(store, path)
  _GGAP_CALL_FUNC("gap.list_store_remove", store, $GtkTreeIndexG2P(path[1]));
end);

InstallMethod(Remove, [IsGtkListStore, IsPosInt],
function(store, pos)
  Remove(store, [pos]);
end);

###############################################################################
##
##  SetItem( <store>, <pos>, <item> )
##
InstallMethod(SetItem, [IsGtkListStore, IsList, IsObject],
function(store, path, item)
  _GGAP_CALL_FUNC("gap.list_store_set", store,
                  $GtkTreePathG2P(path),
                  $GtkTreeItemG2P(item));
end);

InstallMethod(SetItem, [IsGtkListStore, IsPosInt, IsObject],
function(store, pos, item)
  SetItem(store, [pos], item);
end);

###############################################################################
##
##  GetItem( <store>, <pos> )
##
InstallMethod(GetItem, [IsGtkListStore, IsList],
function(store, path)
  return $GtkTreeItemP2G(_GGAP_CALL_FUNC("gap.list_store_get", store,
                                         $GtkTreePathG2P(path)));
end);

InstallMethod(GetItem, [IsGtkListStore, IsPosInt],
function(store, pos)
  return GetItem(store, [pos]);
end);

###############################################################################
##
##  SetList( <listobj>, <items> )
##
InstallMethod(SetList, [IsGtkListStore, IsList],
function(store, items)
  _GGAP_CALL_FUNC("gap.list_store_set_list", store,
                  List(items, $GtkTreeItemG2P));
end);

###############################################################################
##
##  GetList( <listobj> )
##
InstallMethod(GetList, [IsGtkListStore],
function(store)
  return List(_GGAP_CALL_FUNC("gap.list_store_get_list", store),
              $GtkTreeItemP2G);
end);



###############################################################################
##
##  GtkTreeView
##

InstallOtherMethod(Length, [IsGtkTreeView],
function(view) return Length(GetModel(view)); end);

InstallOtherMethod(Clear, [IsGtkTreeView],
function(view) Clear(GetModel(view)); end);

InstallOtherMethod(Insert, [IsGtkTreeView, IsObject, IsObject],
function(view, path, value) Insert(GetModel(view), path, value); end);

InstallOtherMethod(Append, [IsGtkTreeView, IsObject],
function(view, value) Append(GetModel(view), value); end);

InstallOtherMethod(Remove, [IsGtkTreeView, IsObject],
function(view, path) Remove(GetModel(view), path); end);

InstallOtherMethod(SetItem, [IsGtkTreeView, IsObject, IsObject],
function(view, path, item) SetItem(GetModel(view), path, item); end);

InstallOtherMethod(GetItem, [IsGtkTreeView, IsObject],
function(view, path) return GetItem(GetModel(view), path); end);

InstallOtherMethod(SetList, [IsGtkTreeView, IsObject],
function(view, items) SetList(GetModel(view), items); end);

InstallOtherMethod(GetList, [IsGtkTreeView],
function(view) return GetList(GetModel(view)); end);

###############################################################################
##
##  ScrollToCell( <treeview>, <path>, <column=None>,
##                <use_align=FALSE>, <row_align=0.0>, <col_align=0.0> )
##
_GInstallMethodsOptArgs(ScrollToCell, [IsGtkTreeView, IsObject], [GNone, false, 0, 0], true);

InstallMethod(ScrollToCell, [IsGtkTreeView, IsObject, IsGtkTreeViewColumn, IsBool, IsRat, IsRat],
function(tree_view, path, column, use_align, row_align, col_align)
  _GGAP_CALL_METH(tree_view, "scroll_to_cell", $GtkTreePathG2P(path),
                  column, use_align, row_align, col_align);
end);

InstallMethod(ScrollToCell, [IsGtkTreeView, IsObject, IsGNone, IsBool, IsRat, IsRat],
function(tree_view, path, column, use_align, row_align, col_align)
  _GGAP_CALL_METH(tree_view, "scroll_to_cell", $GtkTreePathG2P(path),
                  column, use_align, row_align, col_align);
end);


###############################################################################
##
##  SetCursor( <treeview>, <path>, <focus_column=None>, <start_editing=False> )
##
_GInstallMethodsOptArgs(SetCursor, [IsGtkTreeView, IsObject], [GNone, false], true);

InstallMethod(SetCursor, [IsGtkTreeView, IsObject, IsGtkTreeViewColumn, IsBool],
function(tree_view, path, focus_column, start_editing)
  _GGAP_CALL_METH(tree_view, "set_cursor", $GtkTreePathG2P(path), focus_column, start_editing);
end);

InstallMethod(SetCursor, [IsGtkTreeView, IsObject, IsGNone, IsBool],
function(tree_view, path, focus_column, start_editing)
  _GGAP_CALL_METH(tree_view, "set_cursor", $GtkTreePathG2P(path), focus_column, start_editing);
end);


###############################################################################
##
##  SetCursorOnCell( <treeview>, <path>, <focus_column=None>, <focus_cell=None>,
##                   <start_editing=False> )
##
_GInstallMethodsOptArgs(SetCursorOnCell, [IsGtkTreeView, IsObject],
                        [GNone, GNone, false], true);

InstallMethod(SetCursorOnCell, [IsGtkTreeView, IsObject, IsGtkTreeViewColumn, IsGtkCellRenderer, IsBool],
function(tree_view, path, focus_column, focus_cell, start_editing)
  _GGAP_CALL_METH(tree_view, "set_cursor", $GtkTreePathG2P(path), focus_column, focus_cell, start_editing);
end);

InstallMethod(SetCursorOnCell, [IsGtkTreeView, IsObject, IsGtkTreeViewColumn, IsGNone, IsBool],
function(tree_view, path, focus_column, focus_cell, start_editing)
  _GGAP_CALL_METH(tree_view, "set_cursor", $GtkTreePathG2P(path), focus_column, focus_cell, start_editing);
end);

InstallMethod(SetCursorOnCell, [IsGtkTreeView, IsObject, IsGNone, IsGtkCellRenderer, IsBool],
function(tree_view, path, focus_column, focus_cell, start_editing)
  _GGAP_CALL_METH(tree_view, "set_cursor", $GtkTreePathG2P(path), focus_column, focus_cell, start_editing);
end);

InstallMethod(SetCursorOnCell, [IsGtkTreeView, IsObject, IsGNone, IsGNone, IsBool],
function(tree_view, path, focus_column, focus_cell, start_editing)
  _GGAP_CALL_METH(tree_view, "set_cursor", $GtkTreePathG2P(path), focus_column, focus_cell, start_editing);
end);


###############################################################################
##
##  GetCursor( <treeview> )
##
InstallMethod(GetCursor, [IsGtkTreeView],
function(tree_view)
  local ret;
  ret := _GGAP_CALL_METH(tree_view, "get_cursor");
  return [$GtkTreePosP2G(ret[1]), ret[2]];
end);


###############################################################################
##
##  GetSelected( <tree_view> )
##  GetSelectedRows( <tree_view> )
##  SelectRow( <tree_view>, <path> )
##  UnselectRow( <tree_view>, <path> )
##  RowIsSelected( <tree_view>, <path> )
##  SelectRange( <tree_view>, <start>, <end> )
##
InstallMethod(GetSelected, [IsGtkTreeView],
function(tree_view) return GetSelected(GetSelection(tree_view)); end);

InstallMethod(GetSelectedRows, [IsGtkTreeView],
function(tree_view) return GetSelectedRows(GetSelection(tree_view)); end);

InstallMethod(SelectRow, [IsGtkTreeView, IsObject],
function(tree_view, path) SelectRow(GetSelection(tree_view), path); end);

InstallMethod(UnselectRow, [IsGtkTreeView, IsObject],
function(tree_view, path) UnselectRow(GetSelection(tree_view), path); end);

InstallMethod(RowIsSelected, [IsGtkTreeView, IsObject],
function(tree_view, path) return RowIsSelected(GetSelection(tree_view), path); end);

InstallMethod(SelectRange, [IsGtkTreeView, IsObject, IsObject],
function(tree_view, start, end_) SelectRange(GetSelection(tree_view), start, end_); end);


###############################################################################
##
##  GetSelected( <tree_selection> )
##  GetSelectedRows( <tree_selection> )
##  SelectRow( <tree_selection>, <path> )
##  UnselectRow( <tree_selection>, <path> )
##  RowIsSelected( <tree_selection>, <path> )
##  SelectRange( <tree_selection>, <start>, <end> )
##
InstallMethod(GetSelected, [IsGtkTreeSelection],
function(selection)
  local ret;
  ret := _GGAP_CALL_FUNC("gap.tree_selection_get_selected", selection);
  if ret[2] = GNone then
    return 0;
  else
    return $GtkTreePosP2G(ret[2]);
  fi;
end);

InstallMethod(GetSelectedRows, [IsGtkTreeSelection],
function(selection)
  local ret;
  ret := _GGAP_CALL_METH(selection, "get_selected_rows");
  return List(ret[2], $GtkTreePosP2G);
end);

InstallMethod(SelectRow, [IsGtkTreeSelection, IsObject],
function(selection, path)
  _GGAP_CALL_METH(selection, "select_path", $GtkTreePosG2P(path));
end);

InstallMethod(UnselectRow, [IsGtkTreeSelection, IsObject],
function(selection, path)
  _GGAP_CALL_METH(selection, "unselect_path", $GtkTreePosG2P(path));
end);

InstallMethod(RowIsSelected, [IsGtkTreeSelection, IsObject],
function(selection, path)
  return _GGAP_CALL_METH(selection, "path_is_selected", $GtkTreePosG2P(path));
end);

InstallMethod(SelectRange, [IsGtkTreeSelection, IsObject, IsObject],
function(selection, start, end_)
  _GGAP_CALL_METH(selection, "select_range",
                  $GtkTreePosG2P(start), $GtkTreePosG2P(end_));
end);
