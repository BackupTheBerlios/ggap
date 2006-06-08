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
    Error("stamp '", stamp, "' already in the list");
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
    Error("stamp '", stamp, "' is not in the list");
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
#F  _GGAP_PRINT_ARG(a)
##
InstallGlobalFunction(_GGAP_PRINT_ARG,
function(a)
  local i, keys, string;

  if a = fail then
    return "none";
  elif IsGObject(a) then
    return Concatenation("GapGObject('", a!.id, "')");
  elif IsBool(a) then
    if a then
      return "true";
    else
      return "false";
    fi;
  elif IsInt(a) then
    return String(a);
  elif IsString(a) then
    return Concatenation("'", _GGAP_ESCAPE_STRING(a), "'");
  elif IsList(a) then
    string := "[";
    for i in [1..Length(a)] do
      if i <> 1 then
        Append(string, ", ");
      fi;
      Append(string, _GGAP_PRINT_ARG(a[i]));
    od;
    Append(string, "]");
    return string;
  elif IsRecord(a) then
    string := "{";
    keys := RecNames(a);
    for i in [1..Length(keys)] do
      if i <> 1 then
        Append(string, ", ");
      fi;
      Append(string, Concatenation(ReplacedString(keys[i], "-", "_"), " = "));
      Append(string, _GGAP_PRINT_ARG(a.(keys[i])));
    od;
    Append(string, "}");
    return string;
  else
    Error("unknown argument type: ", a);
  fi;
end);


#############################################################################
##
#F  _GGAP_PRINT_COMMAND(name, args)
##
InstallGlobalFunction(_GGAP_PRINT_COMMAND,
function(name, args)
  local i, code;

  code := Concatenation(name, "(");
  for i in [1..Length(args)] do
    if i <> 1 then
      Append(code, ", ");
    fi;
    Append(code, _GGAP_PRINT_ARG(args[i]));
  od;
  Append(code, ");");

  return code;
end);


#############################################################################
##
#F  _GGAP_GET_STAMP()
##
InstallGlobalFunction(_GGAP_GET_STAMP,
function()
  local stamp, strstamp;

  _GGAP_DATA.stamp := _GGAP_DATA.stamp + 1;

  if _GGAP_DATA.stamp >= 16^4 then
    _GGAP_DATA.stamp := 0;
  fi;

  stamp := _GGAP_DATA.stamp;

  strstamp := HexStringInt(stamp);
  if Length(strstamp) < 4 then
    strstamp := String(strstamp, 4);
  fi;

  return strstamp;
end);


#############################################################################
##
#F  _GGAP_SEND_COMMAND(name, args)
##
InstallGlobalFunction(_GGAP_SEND_COMMAND,
function(name, args)
  local code, strstamp, result;

  strstamp := _GGAP_GET_STAMP();
  Info(InfoGGAP, 5, "_GGAP_SEND_COMMAND: ", strstamp);

  code := _GGAP_PRINT_COMMAND(name, args);
  Info(InfoGGAP, 5, "_GGAP_SEND_COMMAND: ", code);

  _GGAP_ADD_STAMP(strstamp);
  _GGAP_WRITE("g", strstamp, code);

  while true do
    Info(InfoGGAP, 5, "_GGAP_SEND_COMMAND: calling _GGAP_READ");
    _GGAP_READ();
    result := _GGAP_GET_RESULT(strstamp);
    if result <> fail then
      return result{[2..Length(result)]};
    fi;
  od;
end);


#############################################################################
##
#F  _GGAP_DO_COMMAND(name, args)
##
InstallGlobalFunction(_GGAP_DO_COMMAND,
function(name, args)
  local result;

  result := _GGAP_SEND_COMMAND(name, args);

  if result[1] <> _GGAP_STATUS_OK then
    Error(result[2]);
  fi;

  return result{[2..Length(result)]};
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

  Error("unknown type ", name);
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
#F  _GGAP_MAKE_OBJECT()
##
InstallGlobalFunction(_GGAP_MAKE_OBJECT,
function(typename, id)
  local ind, obj;

  ind := _GGAP_LOOKUP_OBJECT(id);

  if ind > 0 then
    return _GGAP_DATA.objects[ind];
  fi;

  Info(InfoGGAP, 4, "_GGAP_MAKE_OBJECT: creating object ", id, " of type ", typename);
  obj := Objectify(_GGAP_GET_TYPE_BY_NAME(typename),
                   rec(id := id, dead := false, callbacks := [],
                       destroy_func := false));
  _GGAP_REGISTER_OBJECT(obj);

  return obj;
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
    Error("object '", id, "' doesn't exist");
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
function(signal, obj, params)
  local data, callbacks, c;

  Info(InfoGGAP, 5, "_GGAP_SIGNAL: ", signal, obj, params);

  if obj!.dead then
    Info(InfoGGAP, 5, "_GGAP_SIGNAL: dead object");
    return;
  fi;

  callbacks := obj!.callbacks;

  for c in callbacks do
    if c.signal = signal then
      Info(InfoGGAP, 5, "_GGAP_SIGNAL: executing callback");
      data := Concatenation([obj], params, c.data);
      CallFuncList(c.func, data);
    fi;
  od;

  Info(InfoGGAP, 5, "_GGAP_SIGNAL: done");
end);


###############################################################################
##
#F  ConnectCallback(<obj>, <signal>, <callback>, ...)
##
InstallGlobalFunction(ConnectCallback,
function(arg)
  local obj, signal, func, c;

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

  _GGAP_DO_COMMAND("GapConnect", [obj, signal, String(c.id)]);

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
  local callbacks, c, i, args;

  if not IsGObject(obj) then
    Error("first argument must be IsGObject");
  fi;
  if not IsString(signal_or_handler) and not IsInt(signal_or_handler) then
    Error("second argument must be a string or callback id");
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

  _GGAP_DO_COMMAND("GapDisconnect", args);

  obj!.callbacks := Difference(obj!.callbacks, callbacks);

  return Length(callbacks);
end);


###############################################################################
##
#F  _GGAP_DESTROY_OBJECT
##
InstallGlobalFunction(_GGAP_DESTROY_OBJECT,
function(obj)
  if not obj!.dead then
    _GGAP_OBJECT_DESTROYED(obj!.id);
    _GGAP_DO_COMMAND("GapDestroy", [obj]);
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


###############################################################################
##
#M  GObjectSetProperty(<obj>, <propname>, <value>)
##
InstallMethod(GObjectSetProperty, [IsGObject and IsGObjectRep, IsString, IsObject],
function(obj, name, value)
  local result, props;

  if obj!.dead then
    Error("object is destroyed");
  fi;

  props := rec();
  props.(name) := value;

  GObjectSetProperty(obj, props);
end);


###############################################################################
##
#M  GObjectSetProperty(<obj>, <props>)
##
InstallOtherMethod(GObjectSetProperty, [IsGObject and IsGObjectRep, IsRecord],
function(obj, props)
  local result;

  if obj!.dead then
    Error("object is destroyed");
  fi;

  _GGAP_DO_COMMAND("GapSetProperty", [obj, props]);
end);


###############################################################################
##
#M  GObjectGetProperty(<obj>, <propname>)
##
InstallMethod(GObjectGetProperty, [IsGObject and IsGObjectRep, IsString],
function(obj, propname)
  local result;

  if obj!.dead then
    Error("object is destroyed");
  fi;

  result := _GGAP_DO_COMMAND("GapGetProperty", [obj, propname]);

  return result[1];
end);


#E
