if IsBoundGlobal("$GGAP_API_VERSION") and $GGAP_API_VERSION = "%s" then
  $GGAP_RESEND_GLOBALS();
elif not IsBoundGlobal("$GGAP_API_VERSION") then
  Read("%s");
else
  $GGAP_RESET();
  Read("%s");
fi;
