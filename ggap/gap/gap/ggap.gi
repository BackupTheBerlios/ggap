#############################################################################
##
#W  ggap.gi                    ggap package                    Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation. See COPYING file that comes with this
##  distribution.
##


#############################################################################
##
##  _GGAP_DATA
##
InstallValue(_GGAP_DATA,
rec(init := false,              # ggap package is initialized
    session_id := 0,            # GAP session id in ggap
    stamp := 0,                 # timestamp used in GAP-ggap communication

    # List of lists of form [obj_id, WeakPointerObj([obj])]
    objects := [],

    do_check_input := true,
    main_level := 0,

    call_stack := [],
    exec_stack := [],

    types := [],
));


#############################################################################
##
##  InfoGGAP
##
InstallValue(InfoGGAP, NewInfoClass("InfoGGAP"));
# SetInfoLevel(InfoGGAP, 10);


#############################################################################
##
##  _GGAP_INIT()
##
InstallGlobalFunction(_GGAP_INIT,
function(out_pipe, in_pipe, session_id, pipehelper)
  local reset_data;

  reset_data := function()
    _GGAP_DATA.init := false;
    _GGAP_DATA.session_id := 0;
    _GGAP_DATA.stamp := 0;
    _GGAP_DATA.objects := [];
    _GGAP_DATA.do_check_input := true;
    _GGAP_DATA.main_level := 0;
    _GGAP_DATA.call_stack := [];
    _GGAP_DATA.exec_stack := [];
  end;

  if _GGAP_DATA.init then
    Info(InfoGGAP, 3, "GGAP package initialized, assuming loaded workspace");
    reset_data();
  else
    Info(InfoGGAP, 3, "initializing GGAP package");
  fi;

  if not IsString(out_pipe) then
    Error("out_pipe argument is not a string: ", out_pipe);
  fi;
  if not IsString(in_pipe) then
    Error("in_pipe argument is not a string: ", in_pipe);
  fi;
  if not IsInt(session_id) or session_id < 0 or session_id > 9999 then
    Error("invalid session id ", session_id);
  fi;

  _GGAP_DATA.out_pipe_name := out_pipe;
  _GGAP_DATA.in_pipe_name := in_pipe;
  _GGAP_DATA.session_id := session_id;
  Info(InfoGGAP, 3, "GGAP session id ", session_id);

  if in_pipe <> "" then
    if ARCH_IS_WINDOWS() then
      _GGAP_DATA.in_pipe := InputOutputLocalProcess(DirectoryCurrent(), pipehelper, [in_pipe]);
    else
      _GGAP_DATA.in_pipe := InputTextFile(in_pipe);
    fi;
    _GGAP_DATA.in_pipe_fd := FileDescriptorOfStream(_GGAP_DATA.in_pipe);
    InstallCharReadHookFunc(_GGAP_DATA.in_pipe, "r", _GGAP_CHECK_INPUT);
  else
    _GGAP_DATA.in_pipe := fail;
  fi;
  if out_pipe <> "" then
    _GGAP_DATA.out_pipe := OutputTextFile(out_pipe, true);
  else
    _GGAP_DATA.out_pipe := fail;
  fi;

  _GGAP_INIT_TYPES();

  _GGAP_DATA.init := true;
end);


#############################################################################
##
##  _GGAP_PEEK_RETURN()
##
##  Retusn last return value received
##
InstallGlobalFunction(_GGAP_PEEK_RETURN,
function()
  if not IsEmpty(_GGAP_DATA.call_stack) then
    return _GGAP_DATA.call_stack[Length(_GGAP_DATA.call_stack)];
  else
    return fail;
  fi;
end);


#############################################################################
##
##  _GGAP_POP_RETURN()
##
##  Returns last return value received and removes it from the stack.
##
InstallGlobalFunction(_GGAP_POP_RETURN,
function()
  return Remove(_GGAP_DATA.call_stack);
end);


#############################################################################
##
##  _GGAP_PUSH_RETURN()
##
##  Records received return value
##
InstallGlobalFunction(_GGAP_PUSH_RETURN,
function(stamp, success, value)
  Add(_GGAP_DATA.call_stack, [stamp, [success, value]]);
end);


#############################################################################
##
##  _GGAP_GET_STAMP()
##
InstallGlobalFunction(_GGAP_GET_STAMP,
function()
  # XXX overflow, session id
  _GGAP_DATA.stamp := _GGAP_DATA.stamp + 1;
  return _GGAP_DATA.stamp;
end);


#############################################################################
##
##  _GGAP_EXEC_FUNC()
##
InstallGlobalFunction(_GGAP_EXEC_FUNC,
function(stamp, func, args, void)
  local SavedOnBreak, retval;

  if IsString(func) then
    if IsBoundGlobal(func) then
      func := ValueGlobal(func);
    else
      _GGAP_SEND_ERROR(stamp, "GAP error: ", func, " is not a function");
      return;
    fi;
  fi;

  if not IsFunction(func) then
    _GGAP_SEND_ERROR(stamp, "GAP error: asked to call non-function ", func);
    return;
  fi;

  Info(InfoGGAP, 3, "Going to execute ", func, " for stamp ", stamp);

  SavedOnBreak := OnBreak;
  OnBreak := function()
    local exec_stack, st;
    # If this is called, it means an error occurred.
    Info(InfoGGAP, 3, "Error during executing ", func, " for stamp ", stamp);

    # Unwind exec_stack
    exec_stack := Reversed(_GGAP_DATA.exec_stack);
    _GGAP_DATA.exec_stack := [];
    for st in exec_stack do
      _GGAP_SEND_ERROR(st, "GAP error");
    od;

    OnBreak := SavedOnBreak;
  end;

  Add(_GGAP_DATA.exec_stack, stamp);
  if not void then
    retval := CallFuncList(func, args);
  else
    CallFuncList(func, args);
    retval := GNone;
  fi;

  # If we got to here, an error still may have occurred,
  # but the stack is empty in that case.
  if not IsEmpty(_GGAP_DATA.exec_stack) and
      _GGAP_DATA.exec_stack[Length(_GGAP_DATA.exec_stack)] = stamp
  then
    # We are good, return value
    Info(InfoGGAP, 3, "Successfully executed ", func, " for stamp ", stamp);
    Remove(_GGAP_DATA.exec_stack);
    _GGAP_SEND_RETURN(stamp, retval);
  fi;
end);


InstallGlobalFunction(_GInstallMethodsOptArgs,
function(oper, args, opt_args, void)
  local i, real_args, func, all_args;

  all_args := Concatenation(args, opt_args);

  for i in [0..Length(opt_args)-1] do
    real_args := List(args);
    if i <> 0 then
      Append(real_args, List([1..i], i -> IsObject));
    fi;

    func := function(arg)
      local j, full_args;
      full_args := List(arg);
      for j in [Length(arg)+1..Length(all_args)] do
        Add(full_args, all_args[j]);
      od;
      if void then
        CallFuncList(oper, full_args);
      else
        return CallFuncList(oper, full_args);
      fi;
    end;

    InstallMethod(oper, real_args, func);
  od;
end);


#E
