#############################################################################
##
#W  ggap.gd                    ggap package                    Yevgen Muntyan
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


#############################################################################
##
#V  _GGAP_DATA
##
##  This record contains various variables for ggap package.
##
DeclareGlobalVariable("_GGAP_DATA", "ggap package internal data");


#############################################################################
##
#V  InfoGGAP
##
DeclareGlobalVariable("InfoGGAP", "ggap info class");


#############################################################################
##
##  Private functions, do not use them
##
DeclareGlobalFunction("_GGAP_INIT");
DeclareGlobalFunction("_GGAP_READ_COMMAND");
DeclareGlobalFunction("_GGAP_CHECK_INPUT");
DeclareGlobalFunction("_GGAP_ESCAPE_STRING");
DeclareGlobalFunction("_GGAP_WRITE_SCRIPT");
DeclareGlobalFunction("_GGAP_WRITE_PYTHON");
DeclareGlobalFunction("_GGAP_WRITE");
DeclareGlobalFunction("_GGAP_READ");
DeclareGlobalFunction("_GGAP_READ_DATA");


#E
