#############################################################################
##
#W  ggap.gd                    ggap package                    Yevgen Muntyan
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
#F  GMain
#F  GMainQuit
##
DeclareGlobalFunction("GMain");
DeclareGlobalFunction("GMainQuit");


#############################################################################
##
#O  GCallFunc
#F  GEval
#F  GExec
##
DeclareOperation("GCallFunc", [IsString]);
DeclareOperation("GCallFunc", [IsString, IsList]);
DeclareOperation("GCallFunc", [IsString, IsList, IsRecord]);
DeclareGlobalFunction("GEval");
DeclareGlobalFunction("GExec");


#############################################################################
##
#V  InfoGGAP
##
DeclareGlobalVariable("InfoGGAP", "ggap info class");


#############################################################################
##
##  _GGAP_DATA
##
##  This record contains various variables for ggap package.
##  Private, do not use it
##
DeclareGlobalVariable("_GGAP_DATA", "ggap package internal data");


#############################################################################
##
##  Private functions, do not use them
##
DeclareGlobalFunction("_GGAP_INIT");
DeclareGlobalFunction("_GGAP_PEEK_RETURN");
DeclareGlobalFunction("_GGAP_POP_RETURN");
DeclareGlobalFunction("_GGAP_PUSH_RETURN");
DeclareGlobalFunction("_GGAP_GET_STAMP");
DeclareGlobalFunction("_GGAP_EXEC_FUNC");


#E
