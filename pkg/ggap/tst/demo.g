#############################################################################
##
## demo.g
##
## Use the following to run it:
## Read(Filename(DirectoriesLibrary("pkg/ggap/tst"), "demo.g"));
##

_GGAPDemoListSelectionChanged :=
function(listview, data)
  local row, demo, file, filename, source;

  row := GetSelectedRow(data.window.list);

  if row[1] <= 0 or row[1] = data.row then
    return;
  fi;

  data.row := row[1];
  demo := data.demos[row[1]];

  filename := Filename(DirectoriesLibrary("pkg/ggap/tst/demo"), demo[2]);
  file := InputTextFile(filename);
  source := ReadAll(file);
  CloseStream(file);

  if source = fail then
    Print("Could not read file ", filename, "\n");
    return;
  fi;

  SetText(data.window.info, demo[3]);
  SetText(data.window.source, source);
end;


_GGAPDemoListRowActivated :=
function(listview, row, data)
  local demo, file, filename, source;

  demo := data.demos[row[1]];

  filename := Filename(DirectoriesLibrary("pkg/ggap/tst/demo"), demo[2]);
  Read(filename);
end;


GGAPDemo := function()
  local window, file, data, names;

  file := Filename(DirectoriesLibrary("pkg/ggap/tst/glade"), "demo.glade");

  if file = fail then
    Error("could not find glade file");
  fi;

  window := CreateGladeWindow(file);
  data := rec(window:=window, file:=file, row:=0);

  SetFont(window.source, "Monospace");
  SetHighlight(window.source, "GAP");

  ConnectCallback(window.list, "selection-changed", _GGAPDemoListSelectionChanged, data);
  ConnectCallback(window.list, "row-activated", _GGAPDemoListRowActivated, data);

  data.demos := [
    ["Message dialogs", "message.g", "Message dialogs"]
  ];
  names := List(data.demos, l->l[1]);
  SetList(window.list, names);
end;


GGAPDemo();
