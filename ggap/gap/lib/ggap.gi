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
##  _GGAP_DATA
##
BindGlobal("_GGAP_DATA",
rec(init := false,              # ggap package is initialized
    session_id := 0,            # GAP session id in ggap
    debug := false,
    original_funcs := rec(),
    original_funcs_stored := false,
));


#############################################################################
##
##  InfoGGAP
##
BindGlobal("InfoGGAP", NewInfoClass("InfoGGAP"));
SetInfoLevel(InfoGGAP, 10);


#############################################################################
##
##  _GGAP_INIT()
##

BindGlobal("_GGAP_INIT_FANCY",
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
      _GGAP_DATA.original_funcs.(name) := ValueGlobal(name);
    fi;
  end;

  restore_global := function(name)
    unbind_global(name);
    if IsBound(_GGAP_DATA.original_funcs.(name)) then
      BindGlobal(name, _GGAP_DATA.original_funcs.(name));
    fi;
  end;

  if not _GGAP_DATA.original_funcs_stored then
    store_global("InfoDoPrint");
    store_global("ColorPrompt");
    _GGAP_DATA.original_funcs_stored := true;
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
      local prompt;
      prompt := CPROMPT();
      Print("@GGAP-PROMPT@", prompt, "\c");
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

BindGlobal("_GGAP_INIT",
function(out_pipe, in_pipe, session_id, pipehelper, fancy)
  local reset_data, create_input_pipe, create_output_pipe;

  reset_data := function()
    _GGAP_DATA.init := false;
    _GGAP_DATA.fancy := false;
    _GGAP_DATA.session_id := 0;
  end;

  if not IsInt(session_id) or session_id < 0 or session_id > 255 then
    Error("invalid session id ", session_id);
  fi;

  if _GGAP_DATA.init then
    Info(InfoGGAP, 3, "GGAP package initialized, assuming loaded workspace");
    reset_data();
  else
    Info(InfoGGAP, 3, "Initializing GGAP package");
  fi;

  _GGAP_INIT_FANCY(fancy);
  _GGAP_DATA.fancy := fancy;
  Info(InfoGGAP, 3, "# fancy: ", fancy);

  _GGAP_DATA.session_id := session_id;
  Info(InfoGGAP, 3, "GGAP session id ", session_id);

  _GGAP_DATA.init := true;
end);


#E
