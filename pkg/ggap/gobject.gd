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


DeclareCategory("IsGObject", IsObject);

DeclareOperation("DestroyGObject", [IsGObject]);
DeclareGlobalFunction("ConnectCallback");
DeclareGlobalFunction("DisconnectCallback");

DeclareGlobalFunction("_GGAP_SEND_COMMAND");
DeclareGlobalFunction("_GGAP_DESTROY_OBJECT");
DeclareGlobalFunction("_GGAP_REGISTER_OBJECT");
DeclareGlobalFunction("_GGAP_LOOKUP_OBJECT");
DeclareGlobalFunction("_GGAP_OBJECT_DESTROYED");
DeclareGlobalFunction("_GGAP_SIGNAL");
DeclareGlobalFunction("_GGAP_GET_TYPE_BY_NAME");


#E
