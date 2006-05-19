#############################################################################
##
#W  ggap.gi                    ggap package                    Yevgen Muntyan
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
#V  _GGAP_DATA
##
InstallValue(_GGAP_DATA,
rec(init := false,              # ggap package is initialized
    stamp := 0,                 # timestamp used in GAP-ggap communication
    objects := [],              # list of all IsGObject objects
    in_read_command := false,   # _GGAP_READ_COMMAND is being executed
    in_check_input := false,    # _GGAP_CHECK_INPUT is being executed

    # SEND_COMMAND may be called recursively, so these two play role
    # of function stack (since commands are sent and results are received
    # asynchronously, there can't be real stack)
    commands := [],             # stamps of commands being sent
    results := [],              # results of commands being sent

    types := [
      ["Window", IsWindow],
      ["Object", IsGObject],
      ["Widget", IsWidget],
      ["Entry", IsEntry],
      ["MenuItem", IsMenuItem],
      ["CheckMenuItem", IsCheckMenuItem],
      ["Button", IsButton],
      ["ToggleButton", IsToggleButton],
      ["Canvas", IsCanvas],
      ["Statusbar", IsStatusbar]
    ]));


#############################################################################
##
#V  InfoGGAP
##
InstallValue(InfoGGAP, NewInfoClass("InfoGGAP"));


#############################################################################
##
#F  _GGAP_INIT(<out_pipe>, <in_pipe>)
##
InstallGlobalFunction(_GGAP_INIT,
function(out_pipe, in_pipe, pipehelper)
  if _GGAP_DATA.init then
    Error("GGAP package is already initialized");
  fi;

  if not IsString(out_pipe) then
    Error("argument is not a string");
  fi;
  if not IsString(in_pipe) then
    Error("argument is not a string");
  fi;

  _GGAP_DATA.out_pipe_name := out_pipe;
  _GGAP_DATA.in_pipe_name := in_pipe;

  if in_pipe <> "" then
    if ARCH_IS_WINDOWS() then
      _GGAP_DATA.in_pipe := InputOutputLocalProcess(DirectoryCurrent(), pipehelper, [in_pipe]);
    else
      _GGAP_DATA.in_pipe := InputTextFile(in_pipe);
    fi;
    InstallCharReadHookFunc(_GGAP_DATA.in_pipe, "r", _GGAP_CHECK_INPUT);
  else
    _GGAP_DATA.in_pipe := fail;
  fi;
  if out_pipe <> "" then
    _GGAP_DATA.out_pipe := OutputTextFile(out_pipe, true);
  else
    _GGAP_DATA.out_pipe := fail;
  fi;

  _GGAP_DATA.init := true;
end);


#############################################################################
##
#F  _GGAP_READ_COMMAND()
##
InstallGlobalFunction(_GGAP_READ_COMMAND,
function()
  local cmd;

  if _GGAP_DATA.in_read_command then
    Error("_GGAP_READ_COMMAND caused recursion");
  fi;

  _GGAP_DATA.in_read_command := true;
  cmd := _GGAP_READ_DATA();
  _GGAP_DATA.in_read_command := false;

  Info(InfoGGAP, 5, "got command ", cmd);

  # 1 - exec file
  # 2 - signal
  # 3 - object destroyed
  # 4 - close window
  if cmd[1] = 1 then
    Read(cmd[2]);
  elif cmd[1] = 2 then
    _GGAP_SIGNAL(cmd[2], cmd[3], cmd{[4..Length(cmd)]});
  elif cmd[1] = 3 then
    _GGAP_OBJECT_DESTROYED(cmd[2]);
  elif cmd[1] = 4 then
    _GGAP_CLOSE_WINDOW(cmd[2]);
  else
    Error("Unknown command ", cmd);
  fi;
end);


#############################################################################
##
#F  _GGAP_CHECK_INPUT()
##
InstallGlobalFunction(_GGAP_CHECK_INPUT,
function(whatever)
  if _GGAP_DATA.in_check_input then
    Error("_GGAP_CHECK_INPUT recursed!");
  fi;

  _GGAP_DATA.in_check_input := true;
  _GGAP_READ();
  _GGAP_DATA.in_check_input := false;
end);


#############################################################################
##
#F  _GGAP_READ_DATA()
##
InstallGlobalFunction(_GGAP_READ_DATA,
function()
  local type, s, s1, s2, size, string, i, tuple, len, val;

  Info(InfoGGAP, 5, "_GGAP_READ_DATA");
  type := ReadByte(_GGAP_DATA.in_pipe);

  # these must be kept in sync with gapapp-script.h
  # 0 - command
  # 1 - string
  # 2 - one-byte integer
  # 3 - pair
  # 4 - triple
  # 5 - list
  # 6 - boolean
  # 7 - two-byte integer

  ## ggap sent a command: it may be an async command, READ() must be
  ## resumed after executing it; or it may be code returning call result,
  ## then the code must set _GGAP_DATA.async_result.
  if type = 0 then
    Info(InfoGGAP, 5, "_GGAP_READ_DATA: got command");
    _GGAP_READ_COMMAND();
    Info(InfoGGAP, 5, "_GGAP_READ_DATA: command done");
    return 0;
  ## String: first two bytes are string length; rest is the string itself
  elif type = 1 then
    s1 := ReadByte(_GGAP_DATA.in_pipe);
    s2 := ReadByte(_GGAP_DATA.in_pipe);
    size := s1 * 128 + s2;
    if size = 0 then
      return "";
    else
      string := "";
      for i in [1..size] do
        Add(string, CHAR_INT(ReadByte(_GGAP_DATA.in_pipe)));
      od;
      return string;
    fi;
  ## One byte integer
  elif type = 2 then
    return ReadByte(_GGAP_DATA.in_pipe);
  ## A pair: two consequent values
  elif type = 3 then
    tuple := [];
    tuple[1] := _GGAP_READ_DATA();
    tuple[2] := _GGAP_READ_DATA();
    return tuple;
  ## A triple: three consequent values
  elif type = 4 then
    tuple := [];
    tuple[1] := _GGAP_READ_DATA();
    tuple[2] := _GGAP_READ_DATA();
    tuple[3] := _GGAP_READ_DATA();
    return tuple;
  ## A list: first byte is list length, then go list values
  elif type = 5 then
    tuple := [];
    len := ReadByte(_GGAP_DATA.in_pipe);
    for i in [1..len] do
      tuple[i] := _GGAP_READ_DATA();
    od;
    return tuple;
  ## Boolean: false if zero
  elif type = 6 then
    return ReadByte(_GGAP_DATA.in_pipe) <> 0;
  ## Integer: first byte - sign (minus if non-zero), next two bytes - the value
  elif type = 7 then
    s := ReadByte(_GGAP_DATA.in_pipe);
    s1 := ReadByte(_GGAP_DATA.in_pipe);
    s2 := ReadByte(_GGAP_DATA.in_pipe);
    val := s1 * 128 + s2;
    if s <> 0 then
      val := -val;
    fi;
    return val;
  else
    Error("unknown data type ", type);
  fi;
end);


#############################################################################
##
#F  _GGAP_READ()
##
InstallGlobalFunction(_GGAP_READ,
function()
  local result;

  Info(InfoGGAP, 5, "_GGAP_READ");
  result := _GGAP_READ_DATA();

  if result <> 0 then
    Info(InfoGGAP, 5, "_GGAP_READ: got result for stamp ", result[1]);
    AddSet(_GGAP_DATA.results, result);
  else
    Info(InfoGGAP, 5, "_GGAP_READ: got nothing");
  fi;
end);


#############################################################################
##
#F  _GGAP_WRITE(...)
##
InstallGlobalFunction(_GGAP_WRITE,
function(arg)
  local s;
  for s in arg do
    AppendTo(_GGAP_DATA.out_pipe, s);
  od;
  AppendTo(_GGAP_DATA.out_pipe, "\000");
end);


#############################################################################
##
#F  _GGAP_WRITE_PYTHON(...)
##
InstallGlobalFunction(_GGAP_WRITE_PYTHON,
function(arg)
  local s;
  AppendTo(_GGAP_DATA.out_pipe, "p");
  for s in arg do
    AppendTo (_GGAP_DATA.out_pipe, s);
  od;
  AppendTo(_GGAP_DATA.out_pipe, "\000");
end);


#############################################################################
##
#F  _GGAP_WRITE_SCRIPT(...)
##
InstallGlobalFunction(_GGAP_WRITE_SCRIPT,
function(arg)
  local s;
  AppendTo(_GGAP_DATA.out_pipe, "s");
  for s in arg do
    AppendTo (_GGAP_DATA.out_pipe, s);
  od;
  AppendTo(_GGAP_DATA.out_pipe, "\000");
end);


#############################################################################
##
#F  _GGAP_ESCAPE_STRING(string)
##
InstallGlobalFunction(_GGAP_ESCAPE_STRING,
function(string)
  local c, result;
  result := "";
  for c in string do
    if c = '\\' then
      Append(result, "\\\\");
    elif c = '\"' then
      Append(result, "\\\"");
    else
      Add(result, c);
    fi;
  od;
  return result;
end);


#E
