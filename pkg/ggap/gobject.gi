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
#F  _GGAP_SEND_COMMAND(arg)
##
InstallGlobalFunction(_GGAP_SEND_COMMAND,
function(arg)
    local code, s, stamp, strstamp, result;

    if _GGAP_DATA.async_result <> fail then
      _GGAP_DATA.async_result := fail;
      Error("_GGAP_SEND_COMMAND caused recursion");
    fi;

    if _GGAP_DATA.in_send_command then
      _GGAP_DATA.in_send_command := false;
      Error("_GGAP_SEND_COMMAND caused recursion");
    fi;

    _GGAP_DATA.stamp := _GGAP_DATA.stamp + 1;
    stamp := _GGAP_DATA.stamp;
    strstamp := HexStringInt(stamp);

    code := "";
    for s in arg do
        code := Concatenation(code, s);
    od;

    _GGAP_DATA.in_send_command := true;
    _GGAP_WRITE("g", "SetStamp('", strstamp, "');", code);
    result := _GGAP_READ();
    _GGAP_DATA.in_send_command := false;
    _GGAP_DATA.async_result := fail;

    if not IsList(result) or Length(result) < 2 then
        Error("Unexpected result ", result);
    fi;

    if result[1] <> strstamp then
        Error("Wrong timestamp ", result[1]);
    fi;

    Print("got response: ", result, "\n");
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
  if name = "Window" then
    return NewType(GObjectFamily, IsWindow and IsGObjectRep);
  elif name = "Object" then
    return NewType(GObjectFamily, IsGObject and IsGObjectRep);
  elif name = "Widget" then
    return NewType(GObjectFamily, IsWidget and IsGObjectRep);
  elif name = "Entry" then
    return NewType(GObjectFamily, IsEntry and IsGObjectRep);
  elif name = "MenuItem" then
    return NewType(GObjectFamily, IsMenuItem and IsGObjectRep);
  elif name = "CheckMenuItem" then
    return NewType(GObjectFamily, IsCheckMenuItem and IsGObjectRep);
  elif name = "Button" then
    return NewType(GObjectFamily, IsButton and IsGObjectRep);
  elif name = "ToggleButton" then
    return NewType(GObjectFamily, IsToggleButton and IsGObjectRep);
  elif name = "Canvas" then
    return NewType(GObjectFamily, IsCanvas and IsGObjectRep);
  elif name = "Statusbar" then
    return NewType(GObjectFamily, IsStatusbar and IsGObjectRep);
  else
    Error("Unknown type ", name);
  fi;
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

  result := _GGAP_SEND_COMMAND("Connect('", obj!.id, "', '",
                               signal, "', '", String(c.id), "');");

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
  local callbacks, c, cmd, i, result;

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

  cmd := Concatenation("Disconnect('", obj!.id, "'");
  for c in callbacks do
    Append(cmd, Concatenation(", '", String(c.id), "'"));
  od;
  Append(cmd, ");");

  result := _GGAP_SEND_COMMAND(cmd);

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
    result := _GGAP_SEND_COMMAND("Destroy('", obj!.id, "');");

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
