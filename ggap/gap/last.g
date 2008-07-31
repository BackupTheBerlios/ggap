$GGAP_BIND_NAMES := function(bind)
  local bind_global, unbind_global, store_global,
        restore_global, store_and_bind;

  Info(InfoGGAP, 3, "$GGAP_BIND_NAMES");

  unbind_global := function(name)
    if IsBoundGlobal(name) then
      if IsReadOnlyGlobal(name) then
        MakeReadWriteGlobal(name);
      fi;
      UnbindGlobal(name);
    fi;
  end;

  store_global := function(name)
    if IsBoundGlobal(name) then
      $GGAP_DATA.original_funcs.(name) := ValueGlobal(name);
    fi;
  end;

  bind_global := function(name, value)
    unbind_global(name);
    BindGlobal(name, value);
  end;

  store_and_bind := function(name, value)
    store_global(name);
    bind_global(name, value);
  end;

  restore_global := function(name)
    unbind_global(name);
    if IsBound($GGAP_DATA.original_funcs.(name)) then
      BindGlobal(name, $GGAP_DATA.original_funcs.(name));
    fi;
  end;

  if bind then
#     store_global("InfoDoPrint");
#     bind_global("InfoDoPrint",
#     function(arglist)
#       Print("@GGAP-INFO@");
#       CallFuncList(Print, arglist);
#       Print("\n@GGAP-INFO-END@");
#     end);

    bind_global("PrintPromptHook",
    function()
      if not IsEmpty($GGAP_DATA.next_prompt) then
        Print($GGAP_DATA.next_prompt[1]);
        Remove($GGAP_DATA.next_prompt, 1);
      else
        $GGAP_SEND_DATA("prompt:", CPROMPT());
      fi;
    end);

    store_and_bind("ColorPrompt", function(setting)
      if setting = true then
        Print("# ColorPrompt() ignored\n");
      fi;
    end);

    store_and_bind("ANSI_COLORS", false);
    store_and_bind("PAGER_BUILTIN", $GGAP_PAGER);
    store_and_bind("PAGER_EXTERNAL", $GGAP_PAGER);
#     store_and_bind("SizeScreen", $GGAP_SIZE_SCREEN);
  else
    unbind_global("PrintPromptHook");
    restore_global("ColorPrompt");
    restore_global("PAGER_BUILTIN");
    restore_global("PAGER_EXTERNAL");
  fi;
end;
Add($GGAP_DATA.api, "$GGAP_BIND_NAMES");


#############################################################################
##
#F  $GGAP_INIT()
##
$GGAP_INIT :=
function()
  Info(InfoGGAP, 3, "$GGAP_INIT");
  $GGAP_SEND_DATA("status:started");
  $GGAP_BIND_NAMES(true);
  $GGAP_SEND_GLOBALS();
end;
Add($GGAP_DATA.api, "$GGAP_INIT");

#############################################################################
##
#F  $GGAP_DEINIT()
##
##  Undo stuff done in $GGAP_INIT
##
$GGAP_DEINIT := function()
  $GGAP_BIND_NAMES(false);
  Info(InfoGGAP, 3, "$GGAP_DEINIT");
end;
Add($GGAP_DATA.api, "$GGAP_DEINIT");


#############################################################################
##
#F  $GGAP_RESET
##
##  This will be called if $GGAP_API_VERSION is different
##  from the one used in restore.g
##
$GGAP_RESET := function()
  local name, api;

  Info(InfoGGAP, 3, "$GGAP_RESET");

  $GGAP_DEINIT();

  api := $GGAP_DATA.api;
  for name in api do
    UnbindGlobal(name);
  od;
end;


# Call it already!
$GGAP_INIT();
