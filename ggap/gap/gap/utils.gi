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


#E
