InstallMethod(GladeXML, [IsString, IsRecord, IsObject],
function(filename, callbacks, data)
  local ret, sig;

  ret := _GGAP_CALL_FUNC("gap.glade_xml", filename);

  for sig in ret.signals do
    if data <> GNone then
      ConnectCallback(sig.widget, sig.signal, callbacks.(sig.handler), data);
    else
      ConnectCallback(sig.widget, sig.signal, callbacks.(sig.handler));
    fi;
  od;

  return ret.xml;
end);

InstallMethod(GladeXML, [IsString, IsRecord],
function(filename, callbacks)
  return GladeXML(filename, callbacks, GNone);
end);

InstallMethod(GladeXML, [IsString],
function(filename)
  return GladeXML(filename, rec());
end);
