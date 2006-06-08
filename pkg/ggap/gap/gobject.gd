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
#O  GObjectSetProperty(<obj>, <propname>, <value>)
##
DeclareOperation("GObjectSetProperty", [IsGObject, IsString, IsObject]);


#############################################################################
##
#O  GObjectGetProperty(<obj>, <propname>)
##
DeclareOperation("GObjectGetProperty", [IsGObject, IsString]);


#############################################################################
##
#O  DestroyGObject(<obj>)
##
##  Destroys <obj>. Note, it can't be stopped like CloseWindow.
##
DeclareOperation("DestroyGObject", [IsGObject]);


#############################################################################
##
#O  ConnectCallback(<obj>, <signal>, <func>, [<data>, ...])
#O  DisconnectCallback(<obj>, <callback_id>)
##
DeclareGlobalFunction("ConnectCallback");
DeclareGlobalFunction("DisconnectCallback");


#############################################################################
##
##  Private functions, do not use them
##
DeclareGlobalFunction("_GGAP_SEND_COMMAND");
DeclareGlobalFunction("_GGAP_DO_COMMAND");
DeclareGlobalFunction("_GGAP_PRINT_ARG");
DeclareGlobalFunction("_GGAP_PRINT_COMMAND");
DeclareGlobalFunction("_GGAP_GET_RESULT");
DeclareGlobalFunction("_GGAP_GET_STAMP");
DeclareGlobalFunction("_GGAP_ADD_STAMP");
DeclareGlobalFunction("_GGAP_DESTROY_OBJECT");
DeclareGlobalFunction("_GGAP_REGISTER_OBJECT");
DeclareGlobalFunction("_GGAP_LOOKUP_OBJECT");
DeclareGlobalFunction("_GGAP_MAKE_OBJECT");
DeclareGlobalFunction("_GGAP_OBJECT_DESTROYED");
DeclareGlobalFunction("_GGAP_SIGNAL");
DeclareGlobalFunction("_GGAP_GET_TYPE_BY_NAME");


#E
