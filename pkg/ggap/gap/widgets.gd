#############################################################################
##
#W  widgets.gd                  ggap package                   Yevgen Muntyan
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


DeclareCategory("IsWidget", IsGObject);
DeclareCategory("IsMenuItem", IsWidget);
DeclareCategory("IsCheckMenuItem", IsMenuItem);
DeclareCategory("IsButton", IsWidget);
DeclareCategory("IsToggleButton", IsButton);
DeclareCategory("IsCanvas", IsWidget);
DeclareCategory("IsStatusbar", IsWidget);
DeclareCategory("IsEntry", IsWidget);


#############################################################################
##
#C  IsWindow
##
DeclareCategory("IsWindow", IsWidget);
DeclareGlobalFunction("CloseWindow");


#############################################################################
##
##  GladeWindow - window created from a glade file
##
DeclareCategory("IsGladeWindow", IsWindow);
DeclareGlobalFunction("CreateGladeWindow");
DeclareGlobalFunction("WindowLookupControl");


#############################################################################
##
##  Miscelanneous widgets methods
##
DeclareOperation("IsActive", [IsGObject]);
DeclareOperation("SetActive", [IsGObject, IsBool]);
DeclareOperation("IsVisible", [IsGObject]);
DeclareOperation("SetVisible", [IsGObject, IsBool]);
DeclareOperation("GetText", [IsGObject]);
DeclareOperation("SetText", [IsGObject, IsString]);


#############################################################################
##
##  Private functions, do not use them
##
DeclareGlobalFunction("_GGAP_CLOSE_WINDOW");


#E
