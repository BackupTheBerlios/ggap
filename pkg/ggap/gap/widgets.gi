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


BindGlobal("DIALOG_RESPONSE_DESTROY", -4);
BindGlobal("DIALOG_RESPONSE_OK", -5);
BindGlobal("DIALOG_RESPONSE_CANCEL", -6);
BindGlobal("DIALOG_RESPONSE_CLOSE", -7);
BindGlobal("DIALOG_RESPONSE_YES", -8);
BindGlobal("DIALOG_RESPONSE_NO", -9);
BindGlobal("DIALOG_RESPONSE_APPLY", -10);
BindGlobal("DIALOG_RESPONSE_HELP", -11);

BindGlobal("DIALOG_BUTTONS_NONE", 0);
BindGlobal("DIALOG_BUTTONS_OK", 1);
BindGlobal("DIALOG_BUTTONS_CLOSE", 2);
BindGlobal("DIALOG_BUTTONS_CANCEL", 3);
BindGlobal("DIALOG_BUTTONS_YES_NO", 4);
BindGlobal("DIALOG_BUTTONS_OK_CANCEL", 5);

BindGlobal("DIALOG_INFO", 0);
BindGlobal("DIALOG_WARNING", 1);
BindGlobal("DIALOG_QUESTION", 2);
BindGlobal("DIALOG_ERROR", 3);

BindGlobal("FILE_DIALOG_OPEN", 0);
BindGlobal("FILE_DIALOG_OPEN_ANY", 1);
BindGlobal("FILE_DIALOG_SAVE", 2);
BindGlobal("FILE_DIALOG_OPEN_DIR", 3);


###############################################################################
##
#M  PrintObj()
##
InstallMethod(PrintObj, [IsWidget], function (wid) Print("<Widget '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsWindow], function (wid) Print("<Window '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsGladeWindow], function (wid) Print("<GladeWindow '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsMenuItem], function (wid) Print("<MenuItem '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsCheckMenuItem], function (wid) Print("<CheckMenuItem '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsButton], function (wid) Print("<Button '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsToggleButton], function (wid) Print("<ToggleButton '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsCanvas], function (wid) Print("<Canvas '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsStatusbar], function (wid) Print("<Statusbar '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsTreeView], function (wid) Print("<TreeView '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsTextView], function (wid) Print("<TextView '", wid!.id, "'>"); end);
InstallMethod(PrintObj, [IsHtml], function (wid) Print("<Html '", wid!.id, "'>"); end);


###############################################################################
##
#F  WindowClose(window)
##
InstallGlobalFunction(WindowClose,
function(window)
  local result, close, f;

  if window!.dead then
    return;
  fi;

  close := true;

  for f in window!.destroy_funcs do
    if f(window) then
      close := false;
      break;
    fi;
  od;

  if close then
    GObjectDestroy(window);
  fi;
end);


###############################################################################
##
#F  WindowPresent(window)
##
InstallGlobalFunction(WindowPresent,
function(window)
  _GGAP_DO_COMMAND("GapPresent", [window]);
end);


###############################################################################
##
#F  _GGAP_HIDE_ON_CLOSE(window)
##
InstallGlobalFunction(_GGAP_HIDE_ON_CLOSE,
function(window)
  SetVisible(window, false);
  return true;
end);


###############################################################################
##
#F  WindowSetHideOnClose(window, hide)
##
InstallGlobalFunction(WindowSetHideOnClose,
function(window, hide)
  local f;

  if hide then
    if not _GGAP_HIDE_ON_CLOSE in window!.destroy_funcs then
      Add(window!.destroy_funcs, _GGAP_HIDE_ON_CLOSE);
    fi;
  else
    Remove(window!.destroy_funcs, _GGAP_HIDE_ON_CLOSE);
  fi;
end);


###############################################################################
##
#F  WindowAddCloseHook(window, func)
##
InstallGlobalFunction(WindowAddCloseHook,
function(window, func)
  Add(window!.destroy_funcs, func);
end);


###############################################################################
##
#F  WindowRemoveCloseHook(window, func)
##
InstallGlobalFunction(WindowRemoveCloseHook,
function(window, func)
  Remove(window!.destroy_funcs, func);
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

  WindowClose(_GGAP_DATA.objects[ind]);
end);


###############################################################################
##
#F  GladeWindow()
##
InstallGlobalFunction(GladeWindow,
function(arg)
  local window, result, path, file, root, types;

  if Length(arg) < 1 or Length(arg) > 3 then
    Error("invalid arguments");
  fi;

  file := arg[1];
  root := fail;
  types := rec();

  if Length(arg) = 2 then
    if IsRecord(arg[2]) then
      types := arg[2];
    else
      root := arg[2];
    fi;
  elif Length(arg) = 3 then
    root := arg[2];
    types := arg[3];
  fi;

  if not IsExistingFile(file) then
    path := Concatenation("pkg/ggap/glade/", file);
    path := Filename(DirectoriesLibrary(""), path);

    if path = fail then
      Error("Could not find file ", file);
    fi;

    file := path;
  fi;

  result := _GGAP_DO_COMMAND("GapGladeWindow", [file, root, types]);

  window := Objectify(NewType(GObjectFamily, IsGladeWindow and IsGObjectRep),
                      rec(id := result[1], dead := false, callbacks := [],
                          destroy_funcs := []));
  _GGAP_REGISTER_OBJECT(window);

  return window;
end);


###############################################################################
##
#F  GladeLookupWidget()
##
InstallGlobalFunction(GladeLookupWidget,
function(window, name)
    local control, result, ind;

    if not IsGladeWindow(window) then
      Error("GladeLookupWidget: first argument must be IsGladeWindow");
    fi;

    if not IsString(name) then
      Error("GladeLookupWidget: second argument must be a string");
    fi;

    result := _GGAP_DO_COMMAND("GapGladeLookup", [window, name]);

    if result[1] = "" then
      return fail;
    fi;

    return _GGAP_MAKE_OBJECT(result[2], result[1]);
end);


###############################################################################
##
#M  \.
##
InstallMethod(\., [IsGladeWindow, IsPosInt],
function (window, rnam)
  return GladeLookupWidget(window, NameRNam(rnam));
end);


###############################################################################
##
#F  RunDialog()
##
InstallGlobalFunction(RunDialog,
function(dialog)
  local result;

  if not IsWindow(dialog) then
    Error("RunDialog: argument must be IsWindow");
  fi;

  result := _GGAP_DO_COMMAND("GapRunDialog", [dialog]);

  return result[1];
end);


###############################################################################
##
#F  RunDialogMessage()
##
InstallGlobalFunction(RunDialogMessage,
function(arg)
  local result, secondary, params;

  secondary := fail;
  params := fail;

  if Length(arg) = 4 then
    secondary := arg[3];
    params := arg[4];
  elif Length(arg) = 3 then
    if IsRecord(arg[3]) then
      params := arg[3];
    else
      secondary := arg[3];
    fi;
  fi;

  result := _GGAP_DO_COMMAND("GapRunDialogMessage", [arg[1], arg[2], secondary, params]);

  return result[1];
end);


###############################################################################
##
#F  RunDialogEntry()
#F  RunDialogText()
##
BindGlobal("_RUN_DIALOG_ENTRY",
function(entry, args)
  local result, text, caption, params;

  text := "";
  caption := fail;
  params := fail;

  if Length(args) = 1 then
    if IsRecord(args[1]) then
      params := args[1];
    else
      text := args[1];
    fi;
  elif Length(args) = 2 then
    text := args[1];
    if IsRecord(args[2]) then
      params := args[2];
    else
      caption := args[2];
    fi;
  elif Length(args) = 3 then
    text := args[1];
    caption := args[2];
    params := args[3];
  fi;

  result := _GGAP_DO_COMMAND("GapRunDialogEntry", [entry, text, caption, params]);
  return result[1];
end);

InstallGlobalFunction(RunDialogEntry,
function(arg)
  return _RUN_DIALOG_ENTRY(true, arg);
end);

InstallGlobalFunction(RunDialogText,
function(arg)
  return _RUN_DIALOG_ENTRY(false, arg);
end);


###############################################################################
##
#F  RunDialogFile()
##
InstallGlobalFunction(RunDialogFile,
function(arg)
  local result, type, start, params;

  type := FILE_DIALOG_OPEN;
  start := fail;
  params := fail;

  if Length(arg) = 3 then
    type := arg[1];
    start := arg[2];
    params := arg[3];
  elif Length(arg) = 2 then
    type := arg[1];
    if IsRecord(arg[2]) then
      params := arg[2];
    else
      start := arg[2];
    fi;
  elif Length(arg) = 1 then
    if IsRecord(arg[1]) then
      params := arg[1];
    elif IsInt(arg[1]) then
      type := arg[1];
    else
      start := arg[1];
    fi;
  fi;

  if IsDirectory(start) then
    start := start![1];
  fi;

  result := _GGAP_DO_COMMAND("GapRunDialogFile", [type, start, params]);

  if Length(result) > 0 then
    return result[1];
  else
    return fail;
  fi;
end);


###############################################################################
##
#F  SetVisible()
##
InstallMethod(SetVisible, [IsWidget, IsBool],
function(wid, visible)
  GObjectSetProperty(wid, "visible", visible);
end);


###############################################################################
##
#F  IsActive()
##
BindGlobal("_IS_ACTIVE",
function(wid)
  return GObjectGetProperty(wid, "active");
end);

InstallMethod(IsActive, [IsCheckMenuItem], function(wid) return _IS_ACTIVE(wid); end);
InstallMethod(IsActive, [IsToggleButton], function(wid) return _IS_ACTIVE(wid); end);


###############################################################################
##
#F  SetActive()
##
BindGlobal("_SET_ACTIVE",
function(wid, active)
  GObjectSetProperty(wid, "active", active);
end);

InstallMethod(SetActive, [IsCheckMenuItem, IsBool],
function(wid, active) _SET_ACTIVE(wid, active); end);
InstallMethod(SetActive, [IsToggleButton, IsBool],
function(wid, active) _SET_ACTIVE(wid, active); end);


###############################################################################
##
#M  IsVisible(<widget>)
##
InstallMethod(IsVisible, [IsWidget],
function(wid)
  return GObjectGetProperty(wid, "visible");
end);


###############################################################################
##
#F  GetText()
##
BindGlobal("_GET_TEXT",
function(obj)
  return GObjectGetProperty(obj, "gap-text");
end);

InstallMethod(GetText, [IsEntry], function(wid) return _GET_TEXT(wid); end);
InstallMethod(GetText, [IsTextView], function(wid) return _GET_TEXT(wid); end);


###############################################################################
##
#F  _SET_TEXT()
#M  SetText(<entry>, <text>)
#M  SetText(<text_view>, <text>)
##
BindGlobal("_SET_TEXT",
function(obj, text)
  GObjectSetProperty(obj, "gap-text", text);
end);

InstallMethod(SetText, [IsEntry, IsString], function(wid, text) _SET_TEXT(wid, text); end);
InstallMethod(SetText, [IsTextView, IsString], function(wid, text) _SET_TEXT(wid, text); end);


###############################################################################
##
#M  GetList(<tree>)
##
InstallMethod(GetList, [IsTreeView],
function(wid)
  return GObjectGetProperty(wid, "gap-items");
end);


###############################################################################
##
#M  SetList(<tree>, <list>)
##
InstallMethod(SetList, [IsTreeView, IsList],
function(wid, list)
  GObjectSetProperty(wid, "gap-items", list);
end);


###############################################################################
##
#M  GetSelectedRow()
##
InstallMethod(GetSelectedRow, [IsTreeView],
function(wid)
  local result;
  result := _GGAP_DO_COMMAND("GapGetSelectedRow", [wid]);
  return result[1];
end);


InstallMethod(SelectRow, [IsTreeView, IsList],
function(wid, row)
  _GGAP_DO_COMMAND("GapSelectRow", [wid, row]);
end);

InstallMethod(UnselectRow, [IsTreeView, IsList],
function(wid, row)
  _GGAP_DO_COMMAND("GapUnselectRow", [wid, row]);
end);

InstallMethod(SelectAllRows, [IsTreeView],
function(wid)
  _GGAP_DO_COMMAND("GapSelectAllRows", [wid]);
end);

InstallMethod(UnselectAllRows, [IsTreeView],
function(wid)
  _GGAP_DO_COMMAND("GapUnselectAllRows", [wid]);
end);


###############################################################################
##
#M  SetFont(<widget>, <font>)
##
InstallMethod(SetFont, [IsWidget, IsString],
function(wid, font)
  GObjectSetProperty(wid, "gap-font", font);
end);


###############################################################################
##
#F  _SET_HIGHLIGHT()
##
BindGlobal("_SET_HIGHLIGHT",
function(wid, whatever)
  GObjectSetProperty(wid, "gap-highlight", whatever);
end);

InstallMethod(SetHighlight, [IsTextView, IsBool], function(wid, highlight) _SET_HIGHLIGHT(wid, highlight); end);
InstallOtherMethod(SetHighlight, [IsTextView, IsString], function(wid, lang) _SET_HIGHLIGHT(wid, lang); end);


#E
