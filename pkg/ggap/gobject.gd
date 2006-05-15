#############################################################################
##
#W  gobject.gd                  ggap package                   Yevgen Muntyan
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


###############################################################################
##
#C  IsGObject
##
##  This is a category parent for all gui objects - windows, controls, graphical
##  objects, etc.
##
DeclareCategory("IsGObject", IsObject);


###############################################################################
##
#C  IsGWindow
##
DeclareCategory("IsGWindow", IsGObject);


DeclareGlobalFunction("CreateWindow");
DeclareOperation("DestroyGObject", [IsGObject]);

DeclareGlobalFunction("GGAP_SEND_COMMAND");
DeclareGlobalFunction("GGAP_DESTROY_OBJECT");
DeclareGlobalFunction("GGAP_REGISTER_OBJECT");
DeclareGlobalFunction("GGAP_LOOKUP_OBJECT");
DeclareGlobalFunction("GGAP_OBJECT_DESTROYED");


#E
