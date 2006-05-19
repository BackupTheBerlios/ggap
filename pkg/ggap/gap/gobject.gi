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


#############################################################################
##
#V  _GGAP_STATUS_OK
#V  _GGAP_STATUS_ERROR
##
BindGlobal("_GGAP_STATUS_OK", 0);
BindGlobal("_GGAP_STATUS_ERROR", 1);


#############################################################################
##
#V  GObjectFamily
##
BindGlobal("GObjectFamily", NewFamily("GObjectFamily", IsGObject));


#############################################################################
##
#F  _GGAP_ADD_STAMP(stamp)
##
InstallGlobalFunction(_GGAP_ADD_STAMP,
function(stamp)
  if stamp in _GGAP_DATA.commands then
    Error("_GGAP_ADD_STAMP: stamp already in the list");
  fi;
  AddSet(_GGAP_DATA.commands, stamp);
end);


#############################################################################
##
#F  _GGAP_GET_RESULT(stamp)
##
InstallGlobalFunction(_GGAP_GET_RESULT,
function(stamp)
  local r;

  if not stamp in _GGAP_DATA.commands then
    Error("_GGAP_GET_RESULT: stamp is not in the list");
  fi;

  for r in _GGAP_DATA.results do
    if r[1] = stamp then
      RemoveSet(_GGAP_DATA.commands, stamp);
      RemoveSet(_GGAP_DATA.results, r);
      return r;
    fi;
  od;

  return fail;
end);


#############################################################################
##
#F  _GGAP_SEND_COMMAND(arg)
##
InstallGlobalFunction(_GGAP_SEND_COMMAND,
function(name, args)
  local i, code, a, s, stamp, strstamp, result;

  _GGAP_DATA.stamp := _GGAP_DATA.stamp + 1;
  stamp := _GGAP_DATA.stamp;
  strstamp := HexStringInt(stamp);

  code := Concatenation(name, "(");
  for i in [1..Length(args)] do
    a := args[i];

    if IsGObject(a) then
      s := Concatenation("'", a!.id, "'");
    elif IsBool(a) then
      if a then
        s := "true";
      else
        s := "false";
      fi;
    elif IsInt(a) then
      s := String(a);
    elif IsString(a) then
      s := Concatenation("'", a, "'");
    else
      Error("Unknown argument type: ", a);
    fi;

    if i = 1 then
      code := Concatenation(code, s);
    else
      code := Concatenation(code, ", ", s);
    fi;
  od;
  code := Concatenation(code, ");");

  Info(InfoGGAP, 5, "_GGAP_SEND_COMMAND: ", strstamp);
  _GGAP_ADD_STAMP(strstamp);
  _GGAP_WRITE("g", "SetStamp('", strstamp, "');", code);

  while true do
    Info(InfoGGAP, 5, "_GGAP_SEND_COMMAND: calling _GGAP_READ");
    _GGAP_READ();
    result := _GGAP_GET_RESULT(strstamp);
    if result <> fail then
      return result{[2..Length(result)]};
    fi;
  od;
end);


###############################################################################
##
#R  IsGObjectRep
##
DeclareRepresentation("IsGObjectRep", IsComponentObjectRep and IsAttributeStoringRep,
["id",              # unique object id, set in ggap
 "dead",            # set after the underlying object is destroyed
 "callbacks",       # list of connected callbacks
 "destroy_func"]);  # function which is called when object is about to be destroyed


###############################################################################
##
#M  PrintObj(<obj>)
##
InstallMethod(PrintObj, "method for IsGObject", [IsGObject],
function (obj)
    Print("<Object '", obj!.id, "'>");
end);


###############################################################################
##
#F  _GGAP_REGISTER_OBJECT()
##
InstallGlobalFunction(_GGAP_REGISTER_OBJECT,
function(obj)
  Add(_GGAP_DATA.objects, obj);
end);


###############################################################################
##
#F  _GGAP_GET_TYPE_BY_NAME(name)
##
InstallGlobalFunction(_GGAP_GET_TYPE_BY_NAME,
function(name)
  local t;

  for t in _GGAP_DATA.types do
    if t[1] = name then
      return NewType(GObjectFamily, t[2] and IsGObjectRep);
    fi;
  od;

  Error("Unknown type ", name);
end);


###############################################################################
##
#F  _GGAP_LOOKUP_OBJECT()
##
InstallGlobalFunction(_GGAP_LOOKUP_OBJECT,
function(id)
  local i;

  for i in [1..Length(_GGAP_DATA.objects)] do
    if _GGAP_DATA.objects[i]!.id = id then
      return i;
    fi;
  od;

  return 0;
end);


###############################################################################
##
#F  _GGAP_OBJECT_DESTROYED()
##
InstallGlobalFunction(_GGAP_OBJECT_DESTROYED,
function(id)
  local ind, obj;

  ind := _GGAP_LOOKUP_OBJECT(id);

  if ind = 0 then
    Error("Object '", id, "' doesn't exist");
  fi;

  obj := _GGAP_DATA.objects[ind];
  Remove(_GGAP_DATA.objects, ind);
  obj!.dead := true;
end);


###############################################################################
##
#F  _GGAP_SIGNAL(<obj_id>, <signal>, <data>)
##
InstallGlobalFunction(_GGAP_SIGNAL,
function(id, signal, params)
  local data, ind, obj, callbacks, c;

  ind := _GGAP_LOOKUP_OBJECT(id);

  if ind = 0 then
    Print("Oops\n");
    return;
  fi;

  obj := _GGAP_DATA.objects[ind];
  callbacks := obj!.callbacks;

  for c in callbacks do
    if c.signal = signal then
      data := Concatenation([obj], params, c.data);
      CallFuncList(c.func, data);
    fi;
  od;
end);


###############################################################################
##
#F  ConnectCallback(<obj>, <signal>)
##
InstallGlobalFunction(ConnectCallback,
function(arg)
  local obj, signal, func, c, result;

  if Length(arg) < 3 then
    Error();
  fi;

  obj := arg[1];
  signal := arg[2];
  func := arg[3];

  if not IsGObject(obj) or not IsString(signal) or not IsFunction(func) then
    Error();
  fi;

  c := rec(signal:=signal, func:=func,
           data:=arg{[4..Length(arg)]},
           id:=_GGAP_DATA.stamp);

  result := _GGAP_SEND_COMMAND("Connect", [obj, signal, String(c.id)]);

  if result[1] <> _GGAP_STATUS_OK then
    Error(result[2]);
  fi;

  Add(obj!.callbacks, c);
  return c.id;
end);


###############################################################################
##
#F  DisconnectCallback(<obj>, <signal>)
#F  DisconnectCallback(<obj>, <id>)
##
InstallGlobalFunction(DisconnectCallback,
function(obj, signal_or_handler)
  local callbacks, c, i, result, args;

  if not IsGObject(obj) then
    Error("DisonnectCallback: first argument must be IsGObject");
  fi;
  if not IsString(signal_or_handler) and not IsInt(signal_or_handler) then
    Error("DisonnectCallback: second argument must be a string or callback id");
  fi;

  callbacks := [];

  if IsString(signal_or_handler) then
    for c in obj!.callbacks do
      if c.signal = signal_or_handler then
        Add(callbacks, c);
      fi;
    od;
  else
    for c in obj!.callbacks do
      if c.id = signal_or_handler then
        Add(callbacks, c);
        break;
      fi;
    od;
  fi;

  if IsEmpty(callbacks) then
    return 0;
  fi;

  args := [obj];
  for c in callbacks do
    Add(args, String(c.id));
  od;

  result := _GGAP_SEND_COMMAND("Disconnect", args);

  if result[1] <> _GGAP_STATUS_OK then
    Error(result[2]);
  fi;

  obj!.callbacks := Difference(obj!.callbacks, callbacks);

  return Length(callbacks);
end);


###############################################################################
##
#F  _GGAP_DESTROY_OBJECT
##
InstallGlobalFunction(_GGAP_DESTROY_OBJECT,
function(obj)
    local result;

    if obj!.dead then
      return;
    fi;

    _GGAP_OBJECT_DESTROYED(obj!.id);
    result := _GGAP_SEND_COMMAND("Destroy", [obj]);

    if result[1] <> _GGAP_STATUS_OK then
      Error(result[2]);
    fi;
end);


###############################################################################
##
#M  DestroyGObject(<obj>)
##
InstallMethod(DestroyGObject, "method for IsGObject", [IsGObject and IsGObjectRep],
function(obj)
    _GGAP_DESTROY_OBJECT(obj);
end);


#E
