#############################################################################
##
#W  gobject.gi                  ggap package                   Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##
##  See COPYING file that comes with this distribution.
##


###############################################################################
##
##  IsGObjectRep
##
DeclareRepresentation("IsGObjectRep", IsComponentObjectRep,
["id",        # unique object id, set in ggap
 "dead",      # set after the underlying object has been destroyed
 "callbacks", # list of connected callbacks
 "type",      # type name
]);


#############################################################################
##
##  GObjectFamily
##
BindGlobal("GObjectFamily", NewFamily("GObjectFamily", IsGObject));


#############################################################################
##
##  GNone
##
InstallValue(GNone,
             Objectify(NewType(NewFamily("GNoneFamily", IsGNone),
                               IsGNone and IsPositionalObjectRep), []));
InstallMethod(PrintObj, "method for IsGNone", [IsGNone],
function(obj)
    Print("<None>");
end);


#############################################################################
##
##  GError
##
BindGlobal("GErrorFamily", NewFamily("GErrorFamily", IsGError));
DeclareRepresentation("IsGErrorRep", IsPositionalObjectRep, [1]);
InstallGlobalFunction(GError,
function(arg)
  local msg;
  if IsEmpty(arg) then
    msg := "";
  else
    msg := Concatenation(List(arg, s->String(s)));
  fi;
  return Objectify(NewType(GErrorFamily, IsGError and IsGErrorRep), [msg]);
end);
InstallMethod(PrintObj, "method for IsGError", [IsGError],
function(obj)
  if IsEmpty(obj![1]) then
    Print("<Error>");
  else
    Print("<Error: ", obj![1], ">");
  fi;
end);


###############################################################################
##
##  _GGAP_REGISTER_TYPE(name, category)
##
InstallGlobalFunction(_GGAP_REGISTER_TYPE,
function(name, category)
  Add(_GGAP_DATA.types, [name, NewType(GObjectFamily, category and IsGObjectRep)]);
end);


###############################################################################
##
##  _GGAP_INIT_TYPES(name)
##
InstallGlobalFunction(_GGAP_INIT_TYPES,
function()
  if not IsEmpty(_GGAP_DATA.types) then
    return;
  fi;

  _GGAP_REGISTER_TYPE("GObject", IsGObject);
  _GGAP_REGISTER_WIDGETS();
end);


###############################################################################
##
##  _GGAP_GET_TYPE_BY_NAME(name)
##
InstallGlobalFunction(_GGAP_GET_TYPE_BY_NAME,
function(name)
  local t;

  t := First(_GGAP_DATA.types, elm -> (elm[1] = name));

  if t = fail then
    t := _GGAP_DATA.types[1];
  fi;

  return t[2];
end);


###############################################################################
##
##  PrintObj(<obj>)
##
InstallMethod(PrintObj, "method for IsGObject", [IsGObject],
function (obj)
    Print("<", obj!.type, " ", obj!.id, ">");
end);


#############################################################################
##
##  _GGAP_LOOKUP_OBJECT(id)
##
InstallGlobalFunction(_GGAP_LOOKUP_OBJECT,
function(id)
  local obj, o, i;

  for i in [1..Length(_GGAP_DATA.objects)] do
    o := _GGAP_DATA.objects[i];
    if o[1] = id then
      return ElmWPObj(o[2], 1);
    fi;
  od;

  return fail;
end);


#############################################################################
##
##  _GGAP_WRAP_OBJECT(id, typename)
##
InstallGlobalFunction(_GGAP_WRAP_OBJECT,
function(id, typename)
  local obj, o, i;

  for i in [1..Length(_GGAP_DATA.objects)] do
    o := _GGAP_DATA.objects[i];
    if o[1] = id then
      obj := ElmWPObj(o[2], 1);
      if obj <> fail then
        return obj;
      else
        # dead object resurrected
        Remove(_GGAP_DATA.objects, i);
        break;
      fi;
    fi;
  od;

  Info(InfoGGAP, 4, "_GGAP_WRAP_OBJECT: creating object ", id, " of type ", typename);
  obj := Objectify(_GGAP_GET_TYPE_BY_NAME(typename),
                   rec(id := id, type := typename, dead := false, callbacks := []));
  Add(_GGAP_DATA.objects, [id, WeakPointerObj([obj])]);

  return obj;
end);


#############################################################################
##
##  _GGAP_GC()
##
InstallGlobalFunction(_GGAP_GC,
function()
  local dead, i, o;

  dead := [];

  for i in [1..Length(_GGAP_DATA.objects)] do
    o := _GGAP_DATA.objects[i];
    if ElmWPObj(o[2], 1) = fail then
      Add(dead, o);
      Unbind(_GGAP_DATA.objects[i]);
    fi;
  od;

  if IsEmpty(dead) then
    return [];
  fi;

  _GGAP_DATA.objects := Set(_GGAP_DATA.objects);
  Info(InfoGGAP, 5, "_GGAP_GC: ", Length(dead), " dead objects");
  return List(dead, o -> o[1]);
end);


#############################################################################
##
#O  ConnectCallback(<obj>, <signal>, <func>, [<data>, ...])
#O  DisconnectCallback(<obj>, <callback_id>)
##
InstallGlobalFunction(_GGAP_CONNECT,
function(obj, signal, func, data)
  local result;

  result := _GGAP_CALL_FUNC("__session__.connect", obj!.id, signal);

  if result[1] = 0 then
    return 0;
  fi;

  Add(obj!.callbacks, [result[1], result[2], func, data]);

  return result[1];
end);


InstallGlobalFunction(_GGAP_DISCONNECT,
function(obj, handler_id)
  local cb, i;

  for i in [1..Length(obj!.callbacks)] do
    cb := obj!.callbacks[i];
    if cb[1] = handler_id then
      Remove(obj!.callbacks, i);
      return _GGAP_CALL_FUNC("__session__.disconnect", obj!.id, handler_id);
    fi;
  od;

  return false;
end);


InstallGlobalFunction(_GGAP_CALLBACK,
function(stamp, id, handler_id, args)
  local obj, cb, ret, func, func_data, void, func_args;

  obj := _GGAP_LOOKUP_OBJECT(id);
  ret := 0;
  func := fail;

  if obj = fail then
    _GGAP_SEND_ERROR(stamp, "No object with id ", id);
  fi;

  for cb in obj!.callbacks do
    if cb[1] = handler_id then
      void := cb[2];
      func := cb[3];
      func_data := cb[4];
      break;
    fi;
  od;

  if func = fail then
    _GGAP_SEND_ERROR(stamp, "No handler with id ", handler_id);
  fi;

  func_args := [obj];
  Append(func_args, args);
  Append(func_args, func_data);

  _GGAP_EXEC_FUNC(stamp, func, func_args, void);
end);


InstallMethod(ConnectCallback, "method for IsGObject",
              [IsGObject, IsString, IsFunction],
function(obj, signal, func)
  return _GGAP_CONNECT(obj, signal, func, []);
end);

InstallOtherMethod(ConnectCallback, "method for IsGObject",
                   [IsGObject, IsString, IsFunction, IsObject],
function(obj, signal, func, data)
  return _GGAP_CONNECT(obj, signal, func, [data]);
end);

InstallOtherMethod(ConnectCallback, "method for IsGObject",
                   [IsGObject, IsString, IsFunction, IsObject, IsObject],
function(obj, signal, func, data1, data2)
  return _GGAP_CONNECT(obj, signal, func, [data1, data2]);
end);

InstallOtherMethod(ConnectCallback, "method for IsGObject",
                   [IsGObject, IsString, IsFunction, IsObject, IsObject, IsObject],
function(obj, signal, func, data1, data2, data3)
  return _GGAP_CONNECT(obj, signal, func, [data1, data2, data3]);
end);

InstallMethod(DisconnectCallback, "method for IsGObject", [IsGObject, IsInt],
function(obj, handler_id)
  return _GGAP_DISCONNECT(obj, handler_id);
end);


#E
