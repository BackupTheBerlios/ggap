#############################################################################
##
#W  ggap.gi                    ggap package                    Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or
##  modify it under the terms of the GNU Lesser General Public
##  License as published by the Free Software Foundation. See COPYING file
##  that comes with this distribution.
##


#############################################################################
##
##  $GGAP_DATA
##
BindGlobal("$GGAP_DATA",
rec(init := false,              # ggap package is initialized
    session_id := 0,            # GAP session id in ggap
    debug := false,
    original_funcs := rec(),
    original_funcs_stored := false,
    next_prompt := [],
    globals := [],
    globals_sent := false,
));


#############################################################################
##
##  InfoGGAP
##
BindGlobal("InfoGGAP", NewInfoClass("InfoGGAP"));
SetInfoLevel(InfoGGAP, 10);


#############################################################################
##
##  $GGAP_FORMAT_INT
##
BindGlobal("$GGAP_FORMAT_INT",
function(num)
  local num_s;
  num_s := HexStringInt(num);
  return Concatenation(List([1 .. 8-Length(num_s)], i->'0'), num_s);
end);


#############################################################################
##
##  $GGAP_SEND_DATA()
##
BindGlobal("$GGAP_SEND_DATA",
function(arg)
  local a, len;

  Print("@GGAP@f");

  len := Sum(List(arg, s -> Length(s)));

  Print($GGAP_FORMAT_INT(len));
  for a in arg do
    Print(a);
  od;
  Print("\c");
end);


#############################################################################
##
##  $GGAP_SEND_RESULT_ERROR(stamp, message)
##
BindGlobal("$GGAP_SEND_RESULT_ERROR",
function(stamp, message)
  $GGAP_SEND_DATA("result-error:", $GGAP_FORMAT_INT(stamp), message);
end);

# #############################################################################
# ##
# ##  $GGAP_SEND_RESULT(stamp, ...)
# ##
# BindGlobal("$GGAP_SEND_RESULT",
# function(arg)
#   local list;
#   list := Concatenation(["result:", $GGAP_FORMAT_INT(arg[1])],
#                         arg{2..Length(arg)});
#   CallFuncList($GGAP_SEND_DATA, list);
# end);

BindGlobal("$GGAP_SEND_ADDED_DELETED",
function(added, deleted)
  local w;

  if not IsEmpty(added) then
    Print("@GGAP@vglobals-added:");
    for w in added do
      Print(w, "\n");
    od;
    Print("@GGAP@e\c");
  fi;

  if not IsEmpty(deleted) then
    Print("@GGAP@vglobals-deleted:");
    for w in deleted do
      Print(w, "\n");
    od;
    Print("@GGAP@e\c");
  fi;
end);

BindGlobal("$GGAP_SEND_GLOBALS_FIRST_TIME",
function()
  local current, added, deleted;

  $GGAP_DATA.globals_sent := true;

  current := NamesGVars();
  added := Difference(current, $GGAP_GLOBALS_INIT);
  deleted := Difference($GGAP_GLOBALS_INIT, current);
  $GGAP_SEND_ADDED_DELETED(added, deleted);

  UnbindGlobal("$GGAP_GLOBALS_INIT");

  $GGAP_DATA.globals := List(NamesUserGVars());
end);

BindGlobal("$GGAP_SEND_GLOBALS",
function()
  local current, added, deleted;

  if not $GGAP_DATA.globals_sent then
    $GGAP_SEND_GLOBALS_FIRST_TIME();
    return;
  fi;

  current := NamesUserGVars();
  added := Difference(current, $GGAP_DATA.globals);
  deleted := Difference($GGAP_DATA.globals, current);
  $GGAP_SEND_ADDED_DELETED(added, deleted);

  $GGAP_DATA.globals := List(current);
end);


#############################################################################
##
##  $GGAP_EXEC_COMMAND(stamp, cmdname, ...)
##
BindGlobal("$GGAP_EXEC_COMMAND",
function(arg)
  local stamp, cmd;

  if Length(arg) < 2 then
    $GGAP_SEND_RESULT_ERROR(0, "bad arguments");
    $GGAP_DATA.next_prompt := [""];
    return;
  fi;

  stamp := arg[1];
  cmd := arg[2];

  if cmd = "run-command" then
    if Length(arg) <> 2 then
      $GGAP_SEND_RESULT_ERROR(stamp, "bad arguments for run-command");
      $GGAP_DATA.next_prompt := [""];
    else
      $GGAP_DATA.next_prompt := ["@GGAP@voutput:\c", "@GGAP@e\c"];
    fi;
  elif cmd = "get-globals" then
    $GGAP_SEND_GLOBALS();
    $GGAP_DATA.next_prompt := [""];
  else
    $GGAP_SEND_RESULT_ERROR(stamp, Concatenation("bad command '", String(cmd), "'"));
    $GGAP_DATA.next_prompt := [""];
  fi;
end);


#############################################################################
##
##  $GGAP_INIT_FANCY()
##
BindGlobal("$GGAP_INIT_FANCY",
function(fancy)
  local bind_global, unbind_global, store_global, restore_global;

  unbind_global := function(name)
    if IsBoundGlobal(name) then
      if IsReadOnlyGlobal(name) then
        MakeReadWriteGlobal(name);
      fi;
      UnbindGlobal(name);
    fi;
  end;

  bind_global := function(name, value)
    unbind_global(name);
    BindGlobal(name, value);
  end;

  store_global := function(name)
    if IsBoundGlobal(name) then
      $GGAP_DATA.original_funcs.(name) := ValueGlobal(name);
    fi;
  end;

  restore_global := function(name)
    unbind_global(name);
    if IsBound($GGAP_DATA.original_funcs.(name)) then
      BindGlobal(name, $GGAP_DATA.original_funcs.(name));
    fi;
  end;

  if not $GGAP_DATA.original_funcs_stored then
    store_global("InfoDoPrint");
    store_global("ColorPrompt");
    $GGAP_DATA.original_funcs_stored := true;
  fi;

#   bind_global("InfoDoPrint",
#   function(arglist)
#     Print("@GGAP-INFO@");
#     CallFuncList(Print, arglist);
#     Print("\n@GGAP-INFO-END@\c");
#   end);

  if fancy then
    bind_global("PrintPromptHook",
    function()
      if not IsEmpty($GGAP_DATA.next_prompt) then
        Print($GGAP_DATA.next_prompt[1]);
        Remove($GGAP_DATA.next_prompt, 1);
      else
        $GGAP_SEND_DATA("prompt:", CPROMPT());
      fi;
    end);

    bind_global("ColorPrompt", function(setting)
      if setting = true then
        Print("# ColorPrompt() ignored\n");
      fi;
    end);
  else
    unbind_global("PrintPromptHook");
    restore_global("ColorPrompt");
  fi;
end);


#############################################################################
##
##  $GGAP_INIT()
##
BindGlobal("$GGAP_INIT",
function(out_pipe, in_pipe, session_id, pipehelper, fancy)
  local reset_data, create_input_pipe, create_output_pipe;

  reset_data := function()
    $GGAP_DATA.init := false;
    $GGAP_DATA.fancy := false;
    $GGAP_DATA.session_id := 0;
    $GGAP_DATA.next_prompt := [];
  end;

  if not IsInt(session_id) or session_id < 0 or session_id > 255 then
    Error("invalid session id ", session_id);
  fi;

  if $GGAP_DATA.init then
    Info(InfoGGAP, 3, "GGAP package initialized, assuming loaded workspace");
    reset_data();
  else
    Info(InfoGGAP, 3, "Initializing GGAP package");
  fi;

  $GGAP_INIT_FANCY(fancy);
  $GGAP_DATA.fancy := fancy;
  Info(InfoGGAP, 3, "# fancy: ", fancy);

  $GGAP_DATA.session_id := session_id;
  Info(InfoGGAP, 3, "GGAP session id ", session_id);

  $GGAP_DATA.init := true;
end);


#E
