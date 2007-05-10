###############################################################################
##
##  GladeXML
##

###############################################################################
##
#O  GladeXML( <filename>[, <callbacks>] )
##
InstallMethod(GladeXML, [IsString, IsRecord],
function(filename, callbacks)
  local ret, sig, args, data;

  ret := _GGAP_CALL_FUNC("gap.GladeXML", filename);

  for sig in ret.signals do
    args := [sig.widget, sig.signal];
    data := callbacks.(sig.handler);
    if IsFunction(data) then
      Add(args, data);
    else
      args := Concatenation(args, data);
    fi;
    CallFuncList(ConnectCallback, args);
  od;

  return ret.xml;
end);

InstallMethod(GladeXML, [IsString],
function(filename)
  return GladeXML(filename, rec());
end);

InstallMethod(\., [IsGladeXML, IsPosInt],
function(xml, rnam)
  return GetWidget(xml, NameRNam(rnam));
end);

InstallMethod(IsBound\., [IsGladeXML, IsPosInt],
function(xml, rnam)
  return GetWidget(xml, NameRNam(rnam)) <> GNone;
end);
