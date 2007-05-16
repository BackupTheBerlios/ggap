#############################################################################
##
#W  utils.gi                   ggap package                    Yevgen Muntyan
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
#O  GOpen
##
InstallMethod(GOpen, [IsString],
function(file)
  GOpen(file, -1);
end);

InstallMethod(GOpen, [IsString, IsInt],
function(file, line)
  GExec("moo.edit.editor_instance().open_file_line('",
        _GGAP_ESCAPE_STRING(file), "', ", line, ")");
end);


InstallGlobalFunction(_GDict,
function()
  return [];
end);

InstallGlobalFunction(_GDictInsert,
function(dict, key, value)
  local pos;
  pos := PositionSorted(dict, [key]);
  if pos > Length(dict) or dict[pos][1] <> key then
    Add(dict, [key, value], pos);
  else
    dict[pos] := [key, value];
  fi;
  return pos;
end);

InstallGlobalFunction(_GDictRemove,
function(dict, key)
  local pos, elm;
  pos := PositionSorted(dict, [key]);
  if pos > Length(dict) or dict[pos][1] <> key then
    Error("_GDictRemove: key ", key, " is not in the dict");
  else
    elm := Remove(dict, pos);
  fi;
  return elm[2];
end);

InstallGlobalFunction(_GDictLookup,
function(dict, key)
  local pos, elm;
  pos := PositionSorted(dict, [key]);
  if pos > Length(dict) or dict[pos][1] <> key then
    return fail;
  else
    return dict[pos][2];
  fi;
end);


#E
