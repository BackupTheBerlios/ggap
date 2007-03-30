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


#############################################################################
##
#C  IsGObject
##
##  Parent category for all ggap objects - controls, graphics objects, etc.
##
DeclareCategory("IsGObject", IsObject);


#############################################################################
##
#C  IsGNone
#V  GNone
##
DeclareCategory("IsGNone", IsObject);
DeclareGlobalVariable("GNone", "null");


#############################################################################
##
#C  IsGError
#F  GError
##
DeclareCategory("IsGError", IsObject);
DeclareGlobalFunction("GError");


#############################################################################
##
#O  ConnectCallback(<obj>, <signal>, <func>, [<data>, ...])
#O  DisconnectCallback(<obj>, <callback_id>)
##
DeclareOperation("ConnectCallback", [IsGObject, IsString, IsFunction]);
DeclareOperation("DisconnectCallback", [IsGObject, IsInt]);


#############################################################################
##
##  Private functions, do not use them
##
DeclareGlobalFunction("_GGAP_WRAP_OBJECT");
DeclareGlobalFunction("_GGAP_LOOKUP_OBJECT");
DeclareGlobalFunction("_GGAP_GET_TYPE_BY_NAME");
DeclareGlobalFunction("_GGAP_INIT_TYPES");
DeclareGlobalFunction("_GGAP_REGISTER_TYPE");
DeclareGlobalFunction("_GGAP_GC");
DeclareGlobalFunction("_GGAP_CALLBACK");
DeclareGlobalFunction("_GGAP_CONNECT");
DeclareGlobalFunction("_GGAP_DISCONNECT");


#E
