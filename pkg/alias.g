#############################################################################
##
#W  alias.g                    ggap package                    Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##
##  See COPYING file that comes with this distribution.
##


GGAP_INSTALL_ALIASES :=
function()
    BindGlobal("GGAP_READ", GGAP_API.READ);
    BindGlobal("GGAP_WRITE", GGAP_API.WRITE);
    BindGlobal("GGAP_WRITE_PYTHON", GGAP_API.WRITE_PYTHON);
    BindGlobal("GGAP_WRITE_SCRIPT", GGAP_API.WRITE_SCRIPT);
    BindGlobal("GGAP_ESCAPE_STRING", GGAP_API.ESCAPE_STRING);
    BindGlobal("GRead", GGAP_API.GRead);
    BindGlobal("GExec", GGAP_API.GExec);

    if ARCH_IS_WINDOWS() then
        BindGlobal("GStartFile", GGAP_API.GStartFile);
    fi;
end;
