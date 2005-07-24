#############################################################################
##
#W  ggap.gd                    ggap package                    Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2005 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##
##  See COPYING file that comes with this distribution.
##

Revision.ggap_gd :=
  "@(#)$Id: ggap.gd,v 1.1 2005/02/23 04:44:16 emuntyan Exp $";


#############################################################################
##
#V  GGAP_PARAMS
##
##  This record contains various global variables for ggap package.
##
BindGlobal ("GGAP_PARAMS", rec (
    init := false
));


#############################################################################
##
#F  GGAP_INIT
##
##  Initializes ggap package (most importantly, it sets filename of pipe to
##  ggap)
##
DeclareGlobalFunction("GGAP_INIT");


#############################################################################
##
#F  GGAP_SEND_STRING ([<string1>][, <string2>][, ...])
##
##  Writes its arguments to ggap pipe
##
DeclareGlobalFunction("GGAP_SEND_STRING");
DeclareGlobalFunction("GGAP_ESCAPE_STRING");


#############################################################################
##
#F  GRead (<file>)
##
##  Tells ggap to execute <file> in python interpreter
##
DeclareGlobalFunction("GRead");


#############################################################################
##
#F  GStartFile (<file>)
##
##  Tells ggap to execute <file> as if it were double-clicked
##  Windows only
##
if ARCH_IS_WINDOWS() then
DeclareGlobalFunction("GStartFile");
fi;


#############################################################################
##
#F  GExec (<cmd>)
##
##  Tells ggap to execute <cmd> in a shell
##
DeclareGlobalFunction("GExec");


#E
