#############################################################################
##
#W  widgets.gi                  ggap package                   Yevgen Muntyan
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
#M  PrintObj()
##
InstallMethod(PrintObj, [IsWindow], function (win) Print("<Window '", win!.id, "'>"); end);
InstallMethod(PrintObj, [IsGladeWindow], function (win) Print("<GladeWindow '", win!.id, "'>"); end);
InstallMethod(PrintObj, [IsGraphicsWindow], function (win) Print("<GraphicsWindow '", win!.id, "'>"); end);
InstallMethod(PrintObj, [IsMenuItem], function (win) Print("<MenuItem '", win!.id, "'>"); end);
InstallMethod(PrintObj, [IsCheckMenuItem], function (win) Print("<CheckMenuItem '", win!.id, "'>"); end);
InstallMethod(PrintObj, [IsButton], function (win) Print("<Button '", win!.id, "'>"); end);
InstallMethod(PrintObj, [IsToggleButton], function (win) Print("<ToggleButton '", win!.id, "'>"); end);
InstallMethod(PrintObj, [IsCanvas], function (win) Print("<Canvas '", win!.id, "'>"); end);
InstallMethod(PrintObj, [IsStatusbar], function (win) Print("<Statusbar '", win!.id, "'>"); end);


###############################################################################
##
#F  CloseWindow(window)
##
InstallGlobalFunction(CloseWindow,
function(window)
  local result, close;

  close := true;

  if IsFunction(window!.destroy_func) then
    close := window!.destroy_func(window);
  fi;

  if close then
    DestroyGObject(window);
  fi;
end);


###############################################################################
##
#F  _GGAP_CLOSE_WINDOW(id)
##
InstallGlobalFunction(_GGAP_CLOSE_WINDOW,
function(id)
  local ind;

  ind := _GGAP_LOOKUP_OBJECT(id);

  if ind = 0 then
    Print("oops\n");
    return;
  fi;

  CloseWindow(_GGAP_DATA.objects[ind]);
end);


###############################################################################
##
#F  CreateGladeWindow()
##
InstallGlobalFunction(CreateGladeWindow,
function(file)
    local window, result, path;

    if not IsExistingFile(file) then
      path := Concatenation("pkg/ggap/glade/", file);
      path := Filename(DirectoriesLibrary(""), path);

      if path = fail then
        Error("Could not find file ", file);
        return;
      fi;

      file := path;
    fi;

    result := _GGAP_SEND_COMMAND("CreateGladeWindow('", file, "');");

    if result[1] <> _GGAP_STATUS_OK then
        Error(result[2]);
    fi;

    window := Objectify(NewType(GObjectFamily, IsGladeWindow and IsGObjectRep),
                        rec(id := result[2], dead := false, callbacks := [],
                            destroy_func := false));
    _GGAP_REGISTER_OBJECT(window);

    return window;
end);


###############################################################################
##
#F  WindowLookupControl()
##
InstallGlobalFunction(WindowLookupControl,
function(window, name)
    local control, result, ind;

    if not IsGladeWindow(window) then
      Error("WindowLookupControl: first argument must be IsGladeWindow");
    fi;

    if not IsString(name) then
      Error("WindowLookupControl: second argument must be a string");
    fi;

    result := _GGAP_SEND_COMMAND("GladeLookup('", window!.id, "', '",
                                 name, "');");

    if result[1] <> _GGAP_STATUS_OK then
        Error(result[2]);
    fi;

    if result[2] = "" then
      return fail;
    fi;

    ind := _GGAP_LOOKUP_OBJECT(result[2]);

    if ind > 0 then
      return _GGAP_DATA.objects[ind];
    fi;

    control := Objectify(_GGAP_GET_TYPE_BY_NAME(result[3]),
                         rec(id := result[2], dead := false, callbacks := [],
                            destroy_func := false));
    _GGAP_REGISTER_OBJECT(control);

    return control;
end);


###############################################################################
##
#F  SetVisible()
##
InstallMethod(SetVisible, [IsWidget, IsBool],
function(wid, visible)
  local result;

  if visible then
    result := _GGAP_SEND_COMMAND("Show('", wid!.id, "');");
  else
    result := _GGAP_SEND_COMMAND("Hide('", wid!.id, "');");
  fi;

  if result[1] <> _GGAP_STATUS_OK then
    Error(result[2]);
  fi;
end);


###############################################################################
##
#F  IsActive()
##
BindGlobal("_IS_ACTIVE",
function(id)
  local result;

  result := _GGAP_SEND_COMMAND("IsActive('", id, "');");

  if result[1] <> _GGAP_STATUS_OK then
    Error(result[2]);
  else
    return result[2];
  fi;
end);

InstallMethod(IsActive, [IsCheckMenuItem], function(wid) return _IS_ACTIVE(wid!.id); end);
InstallMethod(IsActive, [IsToggleButton], function(wid) return _IS_ACTIVE(wid!.id); end);


###############################################################################
##
#F  SetActive()
##
InstallMethod(SetActive, [IsWidget, IsBool],
function(wid, active)
  local result;

  if active then
    result := _GGAP_SEND_COMMAND("SetActive('", wid!.id, "', true);");
  else
    result := _GGAP_SEND_COMMAND("SetActive('", wid!.id, "', false);");
  fi;

  if result[1] <> _GGAP_STATUS_OK then
    Error(result[2]);
  fi;
end);


###############################################################################
##
#F  IsVisible()
##
BindGlobal("_IS_VISIBLE",
function(id)
  local result;

  result := _GGAP_SEND_COMMAND("IsVisible('", id, "');");

  if result[1] <> _GGAP_STATUS_OK then
    Error(result[2]);
  else
    return result[2];
  fi;
end);

InstallMethod(IsVisible, [IsWidget], function(wid) return _IS_VISIBLE(wid!.id); end);


###############################################################################
##
#F  GetText()
##
BindGlobal("_GET_TEXT",
function(id)
  local result;

  result := _GGAP_SEND_COMMAND("GetText('", id, "');");

  if result[1] <> _GGAP_STATUS_OK then
    Error(result[2]);
  else
    return result[2];
  fi;
end);

InstallMethod(GetText, [IsEntry], function(wid) return _GET_TEXT(wid!.id); end);


###############################################################################
##
#F  SetText()
##
BindGlobal("_SET_TEXT",
function(id, text)
  local result;

  result := _GGAP_SEND_COMMAND("SetText('", id, "', '",
                               _GGAP_ESCAPE_STRING(text), "');");

  if result[1] <> _GGAP_STATUS_OK then
    Error(result[2]);
  fi;
end);

InstallMethod(SetText, [IsEntry, IsString], function(wid, text) _SET_TEXT(wid!.id, text); end);


#E
