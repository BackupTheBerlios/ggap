#############################################################################
##
## testglade.g
##
## Use the following to run it:
## Read(Filename(DirectoriesLibrary("pkg/ggap/tst"), "testglade.g"));
##

run_dialog1 := function()
  local file, window, entry1, entry2, result;

  file := Filename(DirectoriesLibrary("pkg/ggap/tst/glade"), "file1.glade");
  window := CreateGladeWindow(file);

  SetText(window.entry1, "Some text");
  SetText(window.entry2, "Some more text in the second entry");

  if RunDialog(window) = DIALOG_RESPONSE_OK then
    result := [GetText(window.entry1), GetText(window.entry2)];
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
