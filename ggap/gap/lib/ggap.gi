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
InstallGlobalFunction(_GGAP_INIT,
function(out_pipe, in_pipe, session_id, pipehelper, fancy)
  local reset_data, init_fancy, create_input_pipe, create_output_pipe;

  reset_data := function()
    _GGAP_DATA.init := false;
    _GGAP_DATA.fancy := false;
    _GGAP_DATA.session_id := 0;
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

  if _GGAP_DATA.init then
    Info(InfoGGAP, 3, "GGAP package initialized, assuming loaded workspace");
    reset_data();
  else
    Info(InfoGGAP, 3, "Initializing GGAP package");
  fi;

  if not IsInt(session_id) or session_id < 0 or session_id > 255 then
    Error("invalid session id ", session_id);
  fi;

  _GGAP_DATA.session_id := session_id;
  Info(InfoGGAP, 3, "GGAP session id ", session_id);

  init_fancy(fancy);

  _GGAP_DATA.init := true;
end);


#E
