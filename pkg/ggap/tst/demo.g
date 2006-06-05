#############################################################################
##
## demo.g
##
## Use the following to run it:
## Read(Filename(DirectoriesLibrary("pkg/ggap/tst"), "demo.g"));
##

_GGAPDemoLoad :=
function(no, data)
  local file, filename, source, info;

  if no = 0 then
    filename := Filename(DirectoriesLibrary("pkg/ggap/tst"), "demo.g");
    info := Concatenation(
      "<H2>GGAP Demo</H2>",
      "This pice of code demonstrates possibilities of GAP running in ggap.<p>",
      "Double-click list item on the left to see the code in action, check out ",
      "Source tab to see the code."
    );
  else
    filename := Filename(DirectoriesLibrary("pkg/ggap/tst/demo"), data.demos[no][2]);
    info := data.demos[no][3];
  fi;

  file := InputTextFile(filename);
  source := ReadAll(file);
  CloseStream(file);

  if source = fail then
    Print("Could not read file ", filename, "\n");
    return;
  fi;

  GObjectSetProperty(data.window.info, "markup", info);
  SetText(data.window.source, source);
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

  window := CreateGladeWindow(file, rec(info:="MooHtml"));
  data := rec(window:=window, file:=file, row:=0);

  SetFont(window.source, "Monospace");
  SetHighlight(window.source, "GAP");

  ConnectCallback(window.list, "selection-changed", _GGAPDemoListSelectionChanged, data);
  ConnectCallback(window.list, "row-activated", _GGAPDemoListRowActivated, data);

  data.demos := [
    ["Message dialogs", "message.g", "<H2>Message dialogs</H2>"]
  ];
  names := List(data.demos, l->l[1]);
  SetList(window.list, names);

  _GGAPDemoLoad(0, data);
  return data;
end;


GGAPDemo();
