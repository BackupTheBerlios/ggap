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
##  HideOnDelete( <window> )
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
##  GladeXML( <filename>, <root>, <callbacks>, <types> )
##
InstallOtherMethod(GladeXML, [IsString, IsString, IsRecord, IsRecord],
function(filename, root, callbacks, types)
  local ret, sig, args, data;

  ret := _GGAP_CALL_FUNC("gap.GladeXML", filename, root, types);

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

InstallOtherMethod(GladeXML, [IsString, IsString, IsRecord],
function(filename, root, callbacks)
  return GladeXML(filename, root, callbacks, rec());
end);

InstallOtherMethod(GladeXML, [IsString, IsString],
function(filename, root)
  return GladeXML(filename, root, rec(), rec());
end);

InstallOtherMethod(GladeXML, [IsString, IsRecord, IsRecord],
function(filename, callbacks, types)
  return GladeXML(filename, "", callbacks, types);
end);

InstallOtherMethod(GladeXML, [IsString, IsRecord],
function(filename, callbacks)
  return GladeXML(filename, "", callbacks, rec());
end);

InstallMethod(GladeXML, [IsString],
function(filename)
  return GladeXML(filename, "", rec(), rec());
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

InstallGlobalFunction(GTK_TREE_PATH_G2P,
function(p)
  if IsInt(p) then
    p := [p];
  fi;
  return List(p, i->i-1);
end);

InstallGlobalFunction(GTK_TREE_PATH_P2G,
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

InstallGlobalFunction(GTK_TREE_INDEX_G2P, i -> i-1);

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


# ###############################################################################
# ##
# ##  Canvas items
# ##
# Perform([
#   ["FooCanvasGroup", "foocanvas.Group"],
#   ["FooCanvasLine", "foocanvas.Line"],
#   ["FooCanvasRect", "foocanvas.Rect"],
#   ["FooCanvasEllipse", "foocanvas.Ellipse"],
#   ["FooCanvasWidget", "foocanvas.Widget"],
#   ["FooCanvasPixbuf", "foocanvas.Pixbuf"],
# ], function(cls)
#   local cat, op;
#   op := ValueGlobal(cls[1]);
#   InstallMethod(op, [IsFooCanvasGroup, IsRecord],
#     function(parent, params)
#       return _GGAP_CALL_FUNC_OPTARG(cls[2], parent, params);
#     end);
#   InstallMethod(op, [IsFooCanvas, IsRecord],
#     function(canvas, params)
#       return _GGAP_CALL_FUNC_OPTARG(cls[2], GetRoot(canvas), params);
#     end);
# end);
