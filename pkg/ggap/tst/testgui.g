#############################################################################
##
## testgui.g
##
## Use the following to run it:
## Read(Filename(DirectoriesLibrary("pkg/ggap/tst"), "testgui.g"));
##

run_dialog1 := function()
  local file, window, entry1, entry2, result;

  file := Filename(DirectoriesLibrary("pkg/ggap/tst"), "file2.glade");
  window := CreateGladeWindow(file);
  entry1 := GladeLookupWidget(window, "entry1");
  entry2 := GladeLookupWidget(window, "entry2");

#   SetText(entry1, "Some text");
#   SetText(entry2, "Some more text in the second entry");

  if RunDialog(window) = DIALOG_RESPONSE_OK then
    result := [GetText(entry1), GetText(entry2)];
  else
    result := fail;
  fi;

  CloseWindow(window);
  return result;
end;

result := run_dialog1();

if result <> fail then
  Print("Dialog content: ", result, "\n");
fi;
