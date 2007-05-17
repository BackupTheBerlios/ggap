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

GGAPDemoLoad :=
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

  SetText(data.xml.source, source);
  SetMarkup(data.xml.info, info);
end;


GGAPDemoListSelectionChanged :=
function(selection, data)
  local row, demo, file, filename, source;

  row := GetSelected(selection);

  if row <> 0 and row <> data.row then
    data.row := row;
    GGAPDemoLoad(row, data);
  fi;
end;


GGAPDemoListRowActivated :=
function(listview, row, column, data)
  local demo, filename;

  demo := data.demos[row];

  if demo[4] then
    filename := Filename(DirectoriesLibrary("pkg/ggap/tst"), demo[3]);
    Read(filename);
  fi;
end;


GGAPDemoAbout :=
function(menuitem, data)
  if not IsBound(data.about) then
    data.about := GtkAboutDialog();
    ConnectCallback(data.about, "delete-event", HideOnDelete);
  fi;
  Present(data.about);
end;


GGAPDemo := function()
  local xml, file, data, names, model, column, cell;

  file := Filename(DirectoriesLibrary("pkg/ggap/tst/glade"), "demo.glade");

  if file = fail then
    Error("could not find glade file");
  fi;

  xml := GladeXML(file, "window", rec(), rec(source:="MooTextView"));

  data := rec(xml:=xml, file:=file, row:=0);

  ModifyFont(xml.source, "Monospace");
  SetLangById(xml.source, "GAP");

  ConnectCallback(GetSelection(xml.list), "changed", GGAPDemoListSelectionChanged, data);
  ConnectCallback(xml.list, "row-activated", GGAPDemoListRowActivated, data);
  ConnectCallback(xml.about, "activate", GGAPDemoAbout, data);

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

  model := GtkListStore(IsString);
  SetList(model, names);

  column := GtkTreeViewColumn();
  cell := GtkCellRendererText();
  PackStart(column, cell);
  AddAttribute(column, cell, "text", 0);
  AppendColumn(xml.list, column);
  SetModel(xml.list, model);
  SelectRow(xml.list, 1);

  GMainLoop(xml.window);

  return data;
end;


GGAPDemo();
