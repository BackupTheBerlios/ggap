#############################################################################
##
#W  demo.g                     ggap package                    Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation. See COPYING file that comes with this
##  distribution.
##
##  Use the following to run this file:
##  Read(Filename(DirectoriesLibrary("pkg/ggap/tst"), "demo.g"));
##

_GGAPDemoLoad :=
function(no, data)
  local file, filename, source, info;

  if no = 0 then
    return;
  fi;

  filename := Filename(DirectoriesLibrary("pkg/ggap/tst"), data.demos[no][3]);
  info := data.demos[no][2];
  file := InputTextFile(filename);
  source := ReadAll(file);
  CloseStream(file);

  if source = fail then
    Print("Could not read file ", filename, "\n");
    return;
  fi;

  SetProperty(data.window.info, "markup", info);
  SetProperty(data.window.source, "text", source);
end;


_GGAPDemoListSelectionChanged :=
function(listview, data)
  local row, demo, file, filename, source;

  row := GetSelectedRow(data.window.list);

  if row[1] <= 0 or row[1] = data.row then
    return;
  fi;

  data.row := row[1];
  _GGAPDemoLoad(row[1], data);
end;


_GGAPDemoListRowActivated :=
function(listview, row, data)
  local demo, filename;

  demo := data.demos[row[1]];

  if demo[4] then
    filename := Filename(DirectoriesLibrary("pkg/ggap/tst"), demo[3]);
    Read(filename);
  fi;
end;


_GGAPDemoAbout :=
function(menuitem, data)
  if not IsBound(data.about) then
    data.about := GladeWindow(data.file, "aboutdialog");
    WindowSetHideOnClose(data.about, true);
  fi;
  WindowPresent(data.about);
end;


GGAPDemo := function()
  local window, file, data, names;

  file := Filename(DirectoriesLibrary("pkg/ggap/tst/glade"), "demo.glade");

  if file = fail then
    Error("could not find glade file");
  fi;

  window := GladeWindow(file, "window", rec(info:="MooHtml"));
  data := rec(window:=window, file:=file, row:=0);

  SetFont(window.source, "Monospace");
  SetHighlight(window.source, "GAP");

  ConnectCallback(window.list, "selection-changed", _GGAPDemoListSelectionChanged, data);
  ConnectCallback(window.list, "row-activated", _GGAPDemoListRowActivated, data);
  ConnectCallback(window.about, "activate", _GGAPDemoAbout, data);

  data.demos := [
    ["This demo", # Text that appears in the list
      Concatenation("<H2>GGAP Demo</H2>",
                    "This is a demo of possibilities of GAP running in ggap.<p>",
                    "Double-click list item on the left to see the code in action, check out ",
                    "Source tab to see the code."),
      "demo.g",   # Source file name
      false],     # Whether the file should be Read() on double-click.
    ["Message dialogs",
      Concatenation("<H2>Message dialogs</H2>",
                    "These are used to pop up a window for user feedback."),
      "demo/message.g",
      true],
    ["Entry dialogs",
      Concatenation("<H2>Entry dialogs</H2>",
                    "Dialogs that allows entering text."),
      "demo/entries.g",
      true],
    ["File chooser",
      Concatenation("<H2>File chooser</H2>",
                    "A dialog that allows choosing file(s) or directory."),
      "demo/filechooser.g",
      true]
  ];

  names := List(data.demos, l->l[1]);
  SetList(window.list, names);
  SelectRow(window.list, [1]);

  SetVisible(window, true);

  return data;
end;


GGAPDemo();
