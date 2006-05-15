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


InstallValue(GGAP_PARAMS, rec(init := false, stamp := 0,
                              in_command := false, in_check_input := false,
                              objects := []));
InstallValue(GGAP_API, rec());


#############################################################################
##
#F  GGAP_API.INIT(<out_pipe>, <in_pipe>)
##
GGAP_API.INIT :=
function(out_pipe, in_pipe)
  if GGAP_PARAMS.init then
    Error("GGAP package is already initialized");
  fi;

  if not IsString(out_pipe) then
    Error("argument is not a string");
  fi;
  if not IsString(in_pipe) then
    Error("argument is not a string");
  fi;

  GGAP_PARAMS.out_pipe_name := out_pipe;
  GGAP_PARAMS.in_pipe_name := in_pipe;

  if in_pipe <> "" then
    GGAP_PARAMS.in_pipe := InputTextFile(in_pipe);
    InstallCharReadHookFunc(GGAP_PARAMS.in_pipe, "r", GGAP_API.CHECK_INPUT);
  else
    GGAP_PARAMS.in_pipe := fail;
  fi;
  if out_pipe <> "" then
    GGAP_PARAMS.out_pipe := OutputTextFile(out_pipe, true);
  else
    GGAP_PARAMS.out_pipe := fail;
  fi;

  GGAP_PARAMS.init := true;
end;


#############################################################################
##
#F  GGAP_API.READ_COMMAND()
##
GGAP_API.READ_COMMAND :=
function()
  local cmd;

  if GGAP_PARAMS.in_command then
    Error("GGAP_API.READ_COMMAND caused recursion");
  fi;

  GGAP_PARAMS.in_command := true;
  cmd := GGAP_API.READ();
  GGAP_PARAMS.in_command := false;

  Print("got command ", cmd, "\n");

  # 1 - object destroyed
  if cmd[1] = 1 then
    GGAP_OBJECT_DESTROYED(cmd[2]);
  else
    Error("Unknown command ", cmd);
  fi;
end;


#############################################################################
##
#F  GGAP_API.CHECK_INPUT()
##
GGAP_API.CHECK_INPUT :=
function(whatever)
  if GGAP_PARAMS.in_check_input then
    Error("GGAP_API.CHECK_INPUT recursed!");
  fi;

  GGAP_PARAMS.in_check_input := true;
  GGAP_API.READ();
  GGAP_PARAMS.in_check_input := false;
end;


#############################################################################
##
#F  GGAP_API.READ()
##
GGAP_API.READ :=
function()
  local type, s1, s2, size, string, i, tuple;

  type := ReadByte(GGAP_PARAMS.in_pipe);

  # these must be kept in sync with gapapp-script.h
  # 0 - NONE
  # 1 - string
  # 2 - one byte integer
  # 3 - pair
  # 4 - triple
  # 5 - command

  if type = 0 then
    return;
  elif type = 1 then
    s1 := ReadByte(GGAP_PARAMS.in_pipe);
    s2 := ReadByte(GGAP_PARAMS.in_pipe);
    size := s1 * 256 + s2;
    if size = 0 then
      return "";
    else
      string := "";
      for i in [1..size] do
        Add(string, CHAR_INT(ReadByte(GGAP_PARAMS.in_pipe)));
      od;
      return string;
    fi;
  elif type = 2 then
    return ReadByte(GGAP_PARAMS.in_pipe);
  elif type = 3 then
    tuple := [];
    tuple[1] := GGAP_API.READ();
    tuple[2] := GGAP_API.READ();
    return tuple;
  elif type = 4 then
    tuple := [];
    tuple[1] := GGAP_API.READ();
    tuple[2] := GGAP_API.READ();
    tuple[3] := GGAP_API.READ();
    return tuple;
  elif type = 5 then
    GGAP_API.READ_COMMAND();
    if not GGAP_PARAMS.in_check_input then
      return GGAP_API.READ();
    else
      return;
    fi;
  else
    Error("unknown data type ", type);
  fi;
end;


#############################################################################
##
#F  GGAP_API.WRITE(...)
##
GGAP_API.WRITE :=
function(arg)
  local s;
  for s in arg do
    AppendTo(GGAP_PARAMS.out_pipe, s);
  od;
  AppendTo(GGAP_PARAMS.out_pipe, "\000");
end;


#############################################################################
##
#F  GGAP_API.WRITE_PYTHON(...)
##
GGAP_API.WRITE_PYTHON :=
function(arg)
  local s;
  AppendTo(GGAP_PARAMS.out_pipe, "p");
  for s in arg do
    AppendTo (GGAP_PARAMS.out_pipe, s);
  od;
  AppendTo(GGAP_PARAMS.out_pipe, "\000");
end;


#############################################################################
##
#F  GGAP_API.WRITE_SCRIPT(...)
##
GGAP_API.WRITE_SCRIPT :=
function(arg)
  local s;
  AppendTo(GGAP_PARAMS.out_pipe, "s");
  for s in arg do
    AppendTo (GGAP_PARAMS.out_pipe, s);
  od;
  AppendTo(GGAP_PARAMS.out_pipe, "\000");
end;


#############################################################################
##
#F  GGAP_API.ESCAPE_STRING(string)
##
GGAP_API.ESCAPE_STRING :=
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
end;


#############################################################################
##
#F  GGAP_API.GRead(<file>)
##
##  Tells ggap to execute <file> in python interpreter
##
GGAP_API.GRead :=
function(filename)
  GGAP_API.WRITE("P", filename);
end;


#############################################################################
##
#F  GGAP_API.GStartFile (<file>)
##
##  Tells ggap to execute <file> as if it were double-clicked
##  Windows only
##
if ARCH_IS_WINDOWS() then
GGAP_API.GStartFile :=
function(file)
  GGAP_API.WRITE_PYTHON("import os\nos.startfile(\'",
                        GGAP_API.ESCAPE_STRING (file),
                        "\')\n");
end;
fi;


#############################################################################
##
#F  GGAP_API.GExec (<cmd>)
##
##  Tells ggap to execute <cmd> in a shell
##
GGAP_API.GExec :=
function(cmd)
  GGAP_API.WRITE_PYTHON("import os\nos.system(\'",
                        GGAP_API.ESCAPE_STRING (cmd),
                        "\')\n");
end;


#E
## kate: space-indent on; indent-width 2;
