if IsBoundGlobal("$GGAP_API_VERSION") and $GGAP_API_VERSION = "%1" then
  $GGAP_RESEND_GLOBALS();
  $GGAP_DATA.next_prompt := [];
elif not IsBoundGlobal("$GGAP_API_VERSION") then
  Read("%2");
else
  $GGAP_RESET();
  Read("%3");
fi;
