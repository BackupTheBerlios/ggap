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


BindGlobal("GGAP_STATUS_OK", 0);
BindGlobal("GGAP_STATUS_ERROR", 1);
BindGlobal("GObjectFamily", NewFamily("GObjectFamily", IsGObject));


InstallGlobalFunction(GGAP_SEND_COMMAND,
function(arg)
    local code, s, stamp, strstamp, result;

    GGAP_PARAMS.stamp := GGAP_PARAMS.stamp + 1;
    stamp := GGAP_PARAMS.stamp;
    strstamp := HexStringInt(stamp);

    code := "";
    for s in arg do
        code := Concatenation(code, s);
    od;

    GGAP_API.WRITE("g", "SetStamp('", strstamp, "');", code);

    result := GGAP_API.READ();

    if not IsList(result) or Length(result) <> 3 then
        Error("Unexpected result ", result);
    fi;

    if result[1] <> strstamp then
        Error("Wrong timestamp ", result[1]);
    fi;

    Print("got response: ", result, "\n");
    return [result[2], result[3]];
end);


###############################################################################
##
#R  IsGObjectRep
##
##  GAP representation of GObject wrappers
##
DeclareRepresentation("IsGObjectRep",
                      IsComponentObjectRep and IsAttributeStoringRep,
                      ["id", "dead"]);


InstallMethod(PrintObj, "method for IsGObject", [IsGObject],
function (obj)
    Print("<Object '", obj!.id, "'>");
end);

InstallMethod(PrintObj, "method for IsGWindow", [IsGWindow],
function (win)
    Print("<Window '", win!.id, "'>");
end);


###############################################################################
##
#F  GGAP_REGISTER_OBJECT()
##
InstallGlobalFunction(GGAP_REGISTER_OBJECT,
function(obj)
  Add(GGAP_PARAMS.objects, obj);
end);


###############################################################################
##
#F  GGAP_LOOKUP_OBJECT()
##
InstallGlobalFunction(GGAP_LOOKUP_OBJECT,
function(id)
  local i;

  for i in [1..Length(GGAP_PARAMS.objects)] do
    if GGAP_PARAMS.objects[i]!.id = id then
      return i;
    fi;
  od;

  return 0;
end);


###############################################################################
##
#F  GGAP_OBJECT_DESTROYED()
##
InstallGlobalFunction(GGAP_OBJECT_DESTROYED,
function(id)
  local ind, obj;

  ind := GGAP_LOOKUP_OBJECT(id);

  if ind = 0 then
    Error("Object '", id, "' doesn't exist");
  fi;

  obj := GGAP_PARAMS.objects[ind];
  Remove(GGAP_PARAMS.objects, ind);
  obj!.dead := true;
end);


###############################################################################
##
#F  CreateWindow()
##
InstallGlobalFunction(CreateWindow,
function()
    local window, result;

    result := GGAP_SEND_COMMAND("CreateWindow();");

    if result[1] <> GGAP_STATUS_OK then
        Error(result[2]);
    fi;

    window := Objectify(NewType(GObjectFamily, IsGWindow and IsGObjectRep),
                        rec(id := result[2], dead := false));
    GGAP_REGISTER_OBJECT(window);

    return window;
end);


InstallGlobalFunction(GGAP_DESTROY_OBJECT,
function(obj)
    local result;

    if obj!.dead then
      return;
    fi;

    GGAP_OBJECT_DESTROYED(obj!.id);
    result := GGAP_SEND_COMMAND("Destroy('", obj!.id, "');");

    if result[1] <> GGAP_STATUS_OK then
      Error(result[2]);
    fi;
end);


InstallMethod(DestroyGObject, "method for IsGObject", [IsGObject and IsGObjectRep],
function(obj)
    GGAP_DESTROY_OBJECT(obj);
end);


#E
