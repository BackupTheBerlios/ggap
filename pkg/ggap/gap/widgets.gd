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
DeclareCategory("IsTreeView", IsWidget);
DeclareCategory("IsTextView", IsWidget);
DeclareCategory("IsHtml", IsTextView);


#############################################################################
##
#C  IsWindow
##
DeclareCategory("IsWindow", IsWidget);
DeclareGlobalFunction("WindowPresent");
DeclareGlobalFunction("WindowClose");
DeclareGlobalFunction("WindowSetHideOnClose");
DeclareGlobalFunction("WindowAddCloseHook");
DeclareGlobalFunction("WindowRemoveCloseHook");


#############################################################################
##
##  GladeWindow - window created from a glade file
##
DeclareCategory("IsGladeWindow", IsWindow);
DeclareGlobalFunction("GladeWindow");
DeclareGlobalFunction("GladeLookupWidget");


#############################################################################
##
#F  RunDialog(<dlg>)
##
## Presents the dialog and waits for user response, i.e. until user clicks
## a button or closes the dialog.
## Returns integer representing the response, see DIALOG_RESPONSE_* constants.
##
DeclareGlobalFunction("RunDialog");


#############################################################################
##
#F  RunDialogMessage(type, primary_text, [secondary_text,] [params])
##
## Presents message dialog and waits for user response.
## Returns integer, analogous to RunDialog.
##
DeclareGlobalFunction("RunDialogMessage");


#############################################################################
##
#F  RunDialogEntry(...)
#F  RunDialogText(...)
##
## Runs an entry dialog. DialogEntry is intended for short one-line strings,
## DialogText may contain arbitrary amount of text, broken into lines.
## Returns pair [response, text].
##
DeclareGlobalFunction("RunDialogEntry");
DeclareGlobalFunction("RunDialogText");


#############################################################################
##
#F  RunDialogFile(type, [start_file,] [params])
##
## Runs a file chooser dialog.
## Returns list of filenames chosen or fail.
##
DeclareGlobalFunction("RunDialogFile");


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

DeclareOperation("GetList", [IsTreeView]);
DeclareOperation("SetList", [IsTreeView, IsList]);
DeclareOperation("SelectRow", [IsTreeView, IsList]);
DeclareOperation("UnselectRow", [IsTreeView, IsList]);
DeclareOperation("SelectAllRows", [IsTreeView]);
DeclareOperation("UnselectAllRows", [IsTreeView]);
DeclareOperation("GetSelectedRow", [IsTreeView]);

DeclareOperation("SetFont", [IsGObject, IsString]);
DeclareOperation("SetHighlight", [IsTextView, IsBool]);


#############################################################################
##
##  Private functions, do not use them
##
DeclareGlobalFunction("_GGAP_CLOSE_WINDOW");
DeclareGlobalFunction("_GGAP_HIDE_ON_CLOSE");


#E
