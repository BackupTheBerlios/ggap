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

    # obj_id -> WeakPointerObj([obj])
    objects := _GDict(),
    # obj -> list of callback ids
    callbacks := _GDict(),

    do_check_input := true,
    main_level := 0,

    call_stack := [],
    exec_stack := [],

    types := _GDict(),

    debug := false,
    log_input := [],
    log_output := [],
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
function(out_pipe, in_pipe, session_id, pipehelper, fancy)
  local reset_data, init_fancy, create_input_pipe, create_output_pipe;

  reset_data := function()
    _GGAP_DATA.init := false;
    _GGAP_DATA.fancy := false;
    _GGAP_DATA.session_id := 0;
    _GGAP_DATA.stamp := 0;
    _GGAP_DATA.objects := _GDict();
    _GGAP_DATA.do_check_input := true;
    _GGAP_DATA.main_level := 0;
    _GGAP_DATA.call_stack := [];
    _GGAP_DATA.exec_stack := [];
    _GGAP_DATA.log_input := [];
    _GGAP_DATA.log_output := [];
  end;

  init_fancy := function(fancy)
    local bind_global;

    _GGAP_DATA.fancy := fancy;
    Info(InfoGGAP, 3, "# fancy: ", fancy, "\n");

    bind_global := function(name, value)
      if IsBoundGlobal(name) then
        if IsReadOnlyGlobal(name) then
          MakeReadWriteGlobal(name);
        fi;
        UnbindGlobal(name);
      fi;
      BindGlobal(name, value);
    end;

    if fancy then
      bind_global("PrintPromptHook",
      function()
        local prompt;
        prompt := CPROMPT();
        Info(InfoGGAP, 8, "# prompt: ", prompt, "\n");
        Print("ggap-prompt-", prompt, "\c");
      end);

      bind_global("ColorPrompt", function(setting)
        if setting = true then
          Print("# ColorPrompt() ignored\n");
        fi;
      end);
    fi;
  end;

  create_input_pipe := function(pipehelper, in_pipe)
    if ARCH_IS_WINDOWS() then
      _GGAP_DATA.in_pipe := InputOutputLocalProcess(DirectoryCurrent(), pipehelper, [in_pipe]);
    else
      _GGAP_DATA.in_pipe := InputTextFile(in_pipe);
    fi;
    if _GGAP_DATA.in_pipe = fail then
      Print("WARNING: could not create input pipe. ",
            "Please report this to muntyan@tamu.edu and send the following output.\n");
      Print(LastSystemError(), "\n");
    fi;
    _GGAP_DATA.in_pipe_fd := FileDescriptorOfStream(_GGAP_DATA.in_pipe);
    InstallCharReadHookFunc(_GGAP_DATA.in_pipe, "r", _GGAP_CHECK_INPUT);
  end;

  create_output_pipe := function(out_pipe)
    local count, errors;

    errors := [];
    for count in [1..5] do
      _GGAP_DATA.out_pipe := OutputTextFile(out_pipe, true);
      if _GGAP_DATA.out_pipe = fail then
        Add(errors, LastSystemError());
      else
        break;
      fi;
    od;

    if _GGAP_DATA.out_pipe = fail then
      Print("WARNING: could not create output pipe. ",
            "Please report this to muntyan@tamu.edu and send the following output.\n");
      Print(errors, "\n");
    fi;
  end;

  if _GGAP_DATA.init then
    Info(InfoGGAP, 3, "GGAP package initialized, assuming loaded workspace");
    reset_data();
  else
    Info(InfoGGAP, 3, "Initializing GGAP package");
  fi;

  if not IsString(out_pipe) then
    Error("out_pipe argument is not a string: ", out_pipe);
  fi;
  if not IsString(in_pipe) then
    Error("in_pipe argument is not a string: ", in_pipe);
  fi;
  if not IsInt(session_id) or session_id < 0 or session_id > 255 then
    Error("invalid session id ", session_id);
  fi;

  _GGAP_DATA.out_pipe_name := out_pipe;
  _GGAP_DATA.in_pipe_name := in_pipe;
  _GGAP_DATA.session_id := session_id;
  Info(InfoGGAP, 3, "GGAP session id ", session_id);

  if in_pipe <> "" then
    create_input_pipe(pipehelper, in_pipe);
  else
    _GGAP_DATA.in_pipe := fail;
  fi;

  if out_pipe <> "" then
    create_output_pipe(out_pipe);
  else
    _GGAP_DATA.out_pipe := fail;
  fi;

  init_fancy(fancy);

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
  local stamp;

  _GGAP_DATA.stamp := _GGAP_DATA.stamp + 1;
  stamp := _GGAP_DATA.stamp * 256 + _GGAP_DATA.session_id;

  if stamp >= 10^8 then
    _GGAP_DATA.stamp := 1;
    stamp := _GGAP_DATA.stamp * 256 + _GGAP_DATA.session_id;
  fi;

  return stamp;
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

  Info(InfoGGAP, 3, "Going to execute ", func, " for stamp ", Int(stamp/256));

  SavedOnBreak := OnBreak;
  OnBreak := function()
    local exec_stack, st;
    # If this is called, it means an error occurred.
    Info(InfoGGAP, 3, "Error during executing ", func, " for stamp ", Int(stamp/256));

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
    Info(InfoGGAP, 3, "Calling function for stamp ", Int(stamp/256));
    retval := CallFuncList(func, args);
  else
    Info(InfoGGAP, 3, "Calling void function for stamp ", Int(stamp/256));
    CallFuncList(func, args);
    retval := GNone;
  fi;
  Info(InfoGGAP, 3, "Done calling function for stamp ", Int(stamp/256));

  # If we got to here, an error still may have occurred,
  # but the stack is empty in that case.
  if not IsEmpty(_GGAP_DATA.exec_stack) and
      _GGAP_DATA.exec_stack[Length(_GGAP_DATA.exec_stack)] = stamp
  then
    # We are good, return value
    Info(InfoGGAP, 3, "Successfully executed ", func, " for stamp ", Int(stamp/256));
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

    # To make buildman.pe happy do not call it directly
    CallFuncList(InstallMethod, [oper, real_args, func]);
  od;
end);


#E
