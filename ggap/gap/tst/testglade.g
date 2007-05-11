#############################################################################
##
#W  testglade.g                  ggap package                  Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation. See COPYING file that comes with this
##  distribution.
##
##  Use the following to run this file:
##  Read(Filename(DirectoriesLibrary("pkg/ggap/tst"), "testglade.g"));
##

run_dialog1 := function()
  local xml, file, dialog, result;

  file := Filename(DirectoriesLibrary("pkg/ggap/tst/glade"), "file1.glade");
  xml := GladeXML(file);

  dialog := xml.dialog;

  SetText(xml.entry1, "Some text");
  SetText(xml.entry2, "Some more text in the second entry");

  if Run(dialog) = GTK_RESPONSE_OK then
    result := [GetText(xml.entry1), GetText(xml.entry2)];
  else
    result := fail;
  fi;

  Destroy(dialog);
  return result;
end;

result := run_dialog1();

if result <> fail then
  Print("Dialog content: ", result, "\n");
fi;
