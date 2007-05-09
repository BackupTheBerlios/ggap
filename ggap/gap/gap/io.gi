#############################################################################
##
#W  io.gi                     ggap package                     Yevgen Muntyan
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
#F  GMain()
##
InstallGlobalFunction(GMain,
function()
  local level_here;

  level_here := _GGAP_DATA.main_level + 1;
  _GGAP_DATA.main_level := level_here;

  while _GGAP_DATA.main_level = level_here do
    _GGAP_READ();
  od;
end);


#############################################################################
##
#F  GMainQuit()
##
InstallGlobalFunction(GMainQuit,
function()
  if _GGAP_DATA.main_level <= 0 then
    Error("GMainQuit: no main loop running");
  else
    _GGAP_DATA.main_level := _GGAP_DATA.main_level - 1;
  fi;
end);


#############################################################################
##
#F  _GGAP_CHECK_INPUT()
##
##  Called when there's data available in _GGAP_DATA.in_pipe.
##
InstallGlobalFunction(_GGAP_CHECK_INPUT,
function(whatever)
  if _GGAP_DATA.do_check_input then
    _GGAP_READ();
  fi;
end);


#############################################################################
##
#F  _GGAP_READ_BYTE()
##
##  Reads a byte from the input pipe and converts it to the range [0..255]
##  if needed.
##
InstallGlobalFunction(_GGAP_READ_BYTE,
function()
  local b;

  b := ReadByte(_GGAP_DATA.in_pipe);

  if b < 0 then
    b := b + 256;
  fi;

  return b;
end);


#############################################################################
##
#F  _GGAP_READ_VALUE()
##
##  Reads a value from the input pipe.
##
InstallGlobalFunction(_GGAP_READ_VALUE,
function()
  local type, sign, value, len, i, id, typename,
        read_two_bytes_int;

  type := _GGAP_READ_BYTE();

  read_two_bytes_int := function()
    return 256 * _GGAP_READ_BYTE() + _GGAP_READ_BYTE();
  end;

  # Must be kept in sync with ggap.py

  # python None
  if type = 0 then
    Info(InfoGGAP, 7, "Got None");
    return GNone;

  # Boolean
  elif type = 1 then
    Info(InfoGGAP, 7, "Got bool");
    return _GGAP_READ_BYTE() <> 0;

  # Two-bytes int
  elif type = 2 then
    sign := _GGAP_READ_BYTE() <> 0;
    value := read_two_bytes_int();
    if sign then
      value := -value;
    fi;
    Info(InfoGGAP, 7, "Got int ", value);
    return value;

  # String
  elif type = 3 then
    len := read_two_bytes_int();
    Info(InfoGGAP, 7, "Got string of length ", len);
    if len = 0 then
      return "";
    else
      value := "";
      for i in [1..len] do
        ADD_LIST(value, CHAR_INT(_GGAP_READ_BYTE()));
      od;
      return value;
    fi;

  # List
  elif type = 4 then
    len := read_two_bytes_int();
    Info(InfoGGAP, 7, "Got list of length ", len);
    if len = 0 then
      return [];
    else
      value := [];
      for i in [1..len] do
        ADD_LIST(value, _GGAP_READ_VALUE());
      od;
      return value;
    fi;

  # Object
  elif type = 5 then
    id := _GGAP_READ_VALUE();
    typename := _GGAP_READ_VALUE();
    return _GGAP_WRAP_OBJECT(id, typename);

  # Dict
  elif type = 6 then
    len := read_two_bytes_int();
    Info(InfoGGAP, 7, "Got dict of size ", len/2);
    if len = 0 then
      return rec();
    else
      value := rec();
      for i in [1..len] do
        value.(_GGAP_READ_VALUE()) := _GGAP_READ_VALUE();
      od;
      return value;
    fi;

  # The rest are errors
  else
    Error("_GGAP_READ_VALUE: got unknown data type ", type);
  fi;
end);


#############################################################################
##
#F  _GGAP_READ()
##
##  Reads one chunk of data. It blocks, so it should be called
##  only when there's data available.
##
InstallGlobalFunction(_GGAP_READ,
function()
  local type, stamp, value, message, id, obj, dead, string,
        handler_id, args, func, void;

  Info(InfoGGAP, 5, "_GGAP_READ");

  Info(InfoGGAP, 5, "_GGAP_READ: reading byte");
  type := _GGAP_READ_BYTE();
  Info(InfoGGAP, 5, "_GGAP_READ: got byte ", type);

  if type = fail then
    Error("_GGAP_READ: failed to read from input pipe");
  fi;

  # this must be kept in sync with ggap.py

  # Return value
  if type = 1 then
    stamp := _GGAP_READ_VALUE();
    value := _GGAP_READ_VALUE();
    _GGAP_PUSH_RETURN(stamp, true, value);

  # Error
  elif type = 2 then
    stamp := _GGAP_READ_VALUE();
    message := _GGAP_READ_VALUE();
    _GGAP_PUSH_RETURN(stamp, false, message);

  # Object destroyed
  elif type = 3 then
    id := _GGAP_READ_VALUE();
    obj := _GGAP_LOOKUP_OBJECT(id);
    if obj <> fail then
      obj!.dead := true;
    fi;

  # Collect garbage
  elif type = 4 then
    dead := _GGAP_GC();
    if not IsEmpty(dead) then
      # XXX
      string := "gu[";
      for id in dead do
        APPEND_LIST_INTR(string, String(id));
        APPEND_LIST_INTR(string, ",");
      od;
      APPEND_LIST_INTR(string, "]");
      _GGAP_WRITE(string);
    fi;

  # Callback
  elif type = 5 then
    stamp := _GGAP_READ_VALUE();
    id := _GGAP_READ_VALUE();
    handler_id := _GGAP_READ_VALUE();
    args := _GGAP_READ_VALUE();
    _GGAP_CALLBACK(stamp, id, handler_id, args);

  # Call function
  elif type = 6 then
    stamp := _GGAP_READ_VALUE();
    func := _GGAP_READ_VALUE();
    args := _GGAP_READ_VALUE();
    void := _GGAP_READ_VALUE();
    _GGAP_EXEC_FUNC(stamp, func, args, void);

  # 0 means "nothing", the rest are an error
  elif type <> 0 then
    Error("_GGAP_READ: Got unknown command ", type);
  fi;
end);


#############################################################################
##
#F  _GGAP_WRITE()
##
##  Writes data and terminating zero into the output pipe.
##
InstallGlobalFunction(_GGAP_WRITE,
function(arg)
  local s;
  s := Concatenation(arg);
  Info(InfoGGAP, 6, "_GGAP_WRITE: ", s);
  WriteAll(_GGAP_DATA.out_pipe, s);
  WriteByte(_GGAP_DATA.out_pipe, 0);
end);


#############################################################################
##
#F  _GGAP_SEND_ERROR()
##
InstallGlobalFunction(_GGAP_SEND_ERROR,
function(arg)
  local s, msg;
  s := Concatenation("ge", String(arg[1], 8));
  msg := Concatenation(arg{[2..Length(arg)]});
  APPEND_LIST_INTR(s, _GGAP_SERIALIZE(msg));
  _GGAP_WRITE(s);
end);


#############################################################################
##
#F  _GGAP_SEND_RETURN()
##
InstallGlobalFunction(_GGAP_SEND_RETURN,
function(stamp, value)
  _GGAP_WRITE(Concatenation("gr", String(stamp, 8), _GGAP_SERIALIZE(value)));
end);


#############################################################################
##
#F  _GGAP_WRITE_FUNC()
##
##  Writes data and waits for return value.
##
InstallGlobalFunction(_GGAP_WRITE_FUNC,
function(func, args, kwargs)
  local stamp, entry, string, a, i;

  stamp := _GGAP_GET_STAMP();
  Info(InfoGGAP, 6, "_GGAP_WRITE_FUNC ", stamp, ": ", func, "(", args, ", ", kwargs, ")");

  string := Concatenation(["gf", String(stamp, 8), "[", func, ",["]);
  for a in args do
    APPEND_LIST_INTR(string, a);
    APPEND_LIST_INTR(string, ",");
  od;
  APPEND_LIST_INTR(string, "]");
  if not IsEmpty(kwargs) then
    APPEND_LIST_INTR(string, ",");
    APPEND_LIST_INTR(string, kwargs);
  fi;
  APPEND_LIST_INTR(string, "]");

  _GGAP_WRITE(string);

  while true do
    _GGAP_READ();

    entry := _GGAP_PEEK_RETURN();

    if entry <> fail and entry[1] = stamp then
      _GGAP_POP_RETURN();
      if entry[2][1] then
        return entry[2][2];
      else
        return GError(entry[2][2]);
      fi;
    fi;
  od;
end);


#############################################################################
##
#F  _GGAP_CALL_METH_OPTARG()
##
InstallGlobalFunction(_GGAP_CALL_METH_OPTARG,
function(arg)
  local args, kwargs, a, n, optarg, func;

  func := Concatenation(_GGAP_SERIALIZE(arg[1]), ".", arg[2]);
  optarg := arg[Length(arg)];

  args := [];
  for a in arg{[3..Length(arg)-1]} do
    ADD_LIST(args, _GGAP_SERIALIZE(a));
  od;

  kwargs := "{";
  for n in RecNames(optarg) do
    APPEND_LIST_INTR(kwargs, Concatenation("'", n, "':", _GGAP_SERIALIZE(optarg.(n)), ","));
  od;
  APPEND_LIST_INTR(kwargs, "}");

  return _GGAP_WRITE_FUNC(func, args, kwargs);
end);


#############################################################################
##
#F  _GGAP_CALL_METH()
##
InstallGlobalFunction(_GGAP_CALL_METH,
function(arg)
  local args, obj, meth, func, a;

  func := Concatenation(_GGAP_SERIALIZE(arg[1]), ".", arg[2]);
  args := [];

  for a in arg{[3..Length(arg)]} do
    ADD_LIST(args, _GGAP_SERIALIZE(a));
  od;

  return _GGAP_WRITE_FUNC(func, args, "");
end);


#############################################################################
##
#F  _GGAP_CALL_FUNC_OPTARG()
##
InstallGlobalFunction(_GGAP_CALL_FUNC_OPTARG,
function(arg)
  local args, kwargs, a, optarg, n, func;

  func := arg[1];
  args := [];
  optarg := arg[Length(arg)];

  for a in arg{[2..Length(arg)-1]} do
    ADD_LIST(args, _GGAP_SERIALIZE(a));
  od;

  kwargs := "{";
  for n in RecNames(optarg) do
    APPEND_LIST_INTR(kwargs, Concatenation("'", n, "':", _GGAP_SERIALIZE(optarg.(n)), ","));
  od;
  APPEND_LIST_INTR(kwargs, "}");

  return _GGAP_WRITE_FUNC(func, args, kwargs);
end);


#############################################################################
##
#F  _GGAP_CALL_FUNC()
##
InstallGlobalFunction(_GGAP_CALL_FUNC,
function(arg)
  local args, a;

  args := [];
  for a in arg{[2..Length(arg)]} do
    ADD_LIST(args, _GGAP_SERIALIZE(a));
  od;

  return _GGAP_WRITE_FUNC(arg[1], args, "");
end);


#############################################################################
##
#F  _GGAP_WRITE_RETURN()
##
InstallGlobalFunction(_GGAP_WRITE_RETURN,
function(stamp, value)
  _GGAP_WRITE("gr", String(stamp, 8), value);
end);


#############################################################################
##
#M  GCallFunc()
##
InstallMethod(GCallFunc, [IsString],
function(func)
  return _GGAP_CALL_FUNC(func);
end);

InstallMethod(GCallFunc, [IsString, IsList],
function(func, arg_list)
  local args;
  args := Concatenation([func], arg_list);
  return CallFuncList(_GGAP_CALL_FUNC, args);
end);

InstallMethod(GCallFunc, [IsString, IsList, IsRecord],
function(func, arg_list, kwargs)
  local args;
  args := Concatenation([func], arg_list, [kwargs]);
  return CallFuncList(_GGAP_CALL_FUNC_OPTARG, args);
end);

InstallOtherMethod(GCallFunc, [IsGObject, IsString],
function(obj, meth)
  return _GGAP_CALL_METH(obj, meth);
end);

InstallOtherMethod(GCallFunc, [IsGObject, IsString, IsList],
function(obj, meth, arg_list)
  local args;
  args := Concatenation([obj, meth], arg_list);
  return CallFuncList(_GGAP_CALL_METH, args);
end);

InstallOtherMethod(GCallFunc, [IsGObject, IsString, IsList, IsRecord],
function(obj, meth, arg_list, kwargs)
  local args;
  args := Concatenation([obj, meth], arg_list, [kwargs]);
  return CallFuncList(_GGAP_CALL_METH_OPTARG, args);
end);


#############################################################################
##
#F  GEval()
##
InstallGlobalFunction(GEval,
function(code)
  if not IsString(code) then
    Error("GEval: argument must be a string");
  fi;
  return GCallFunc("eval", [code]);
end);


#############################################################################
##
#F  GExec()
##
InstallGlobalFunction(GExec,
function(arg)
  local string;
  if IsEmpty(arg) then
    return;
  fi;
  _GGAP_WRITE("gc", Concatenation(List(arg, a -> String(a))));
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
      APPEND_LIST_INTR(result, "\\\\");
    elif c = '\"' then
      APPEND_LIST_INTR(result, "\\\"");
    elif c = '\'' then
      APPEND_LIST_INTR(result, "\\\'");
    elif c = '\n' then
      APPEND_LIST_INTR(result, "\\n");
    elif c = '\r' then
      APPEND_LIST_INTR(result, "\\r");
    elif c = '\t' then
      APPEND_LIST_INTR(result, "\\t");
    else
      ADD_LIST(result, c);
    fi;
  od;
  return result;
end);


#############################################################################
##
#F  _GGAP_SERIALIZE()
##
InstallGlobalFunction(_GGAP_SERIALIZE,
function(value)
  local string, elm;

  if IsBool(value) then
    if value then
      return "True";
    else
      return "False";
    fi;

  elif IsGNone(value) then
    return "None";

  elif IsRat(value) then
    return String(value);

  elif IsString(value) then
    return Concatenation("'", _GGAP_ESCAPE_STRING(value), "'");

  elif IsGObject(value) then
    return Concatenation("OBJECT(", String(value!.id), ")");

  elif IsList(value) then
    string := "(";
    for elm in value do
      APPEND_LIST_INTR(string, _GGAP_SERIALIZE(elm));
      APPEND_LIST_INTR(string, ",");
    od;
    APPEND_LIST_INTR(string, ")");
    return string;

  elif IsRecord(value) then
    string := "{";
    for elm in RecNames(value) do
      APPEND_LIST_INTR(string, _GGAP_SERIALIZE(elm));
      APPEND_LIST_INTR(string, ":");
      APPEND_LIST_INTR(string, _GGAP_SERIALIZE(value.(elm)));
      APPEND_LIST_INTR(string, ",");
    od;
    APPEND_LIST_INTR(string, "}");
    return string;

  else
    Error("Can't serialize object ", value);
  fi;
end);


#E
