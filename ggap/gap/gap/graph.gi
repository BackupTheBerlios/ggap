#############################################################################
##
#W  graph.gi                   ggap package                    Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation. See COPYING file that comes with this
##  distribution.
##


InstallGlobalFunction(GGAPGraphicsMode,
function()
  return _GGAP_DATA.fancy;
end);

InstallGlobalFunction(DisplayGraph,
function(obj)
  if GGAPGraphicsMode() then
    DisplayGraphOp(obj);
  else
    Display(obj);
  fi;
end);

InstallMethod(DisplayGraphOp, [IsObject],
function(obj)
  Display(obj);
end);


InstallMethod(DisplayGraphOp, [IsFooCanvas],
function(canvas)
  _GGAP_CALL_FUNC("DISPLAY_GRAPH", canvas);
end);
