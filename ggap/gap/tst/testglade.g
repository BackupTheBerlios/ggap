#############################################################################
##
## testglade.g
##
## Use the following to run it:
## Read(Filename(DirectoriesLibrary("pkg/ggap/tst"), "testglade.g"));
##

run_dialog1 := function()
  local xml, file, dialog, entry1, entry2, result;

  file := Filename(DirectoriesLibrary("pkg/ggap/tst/glade"), "file1.glade");
  xml := GladeXML(file);

  dialog := GetWidget(xml, "dialog");
  entry1 := GetWidget(xml, "entry1");
  entry2 := GetWidget(xml, "entry2");

  SetText(entry1, "Some text");
  SetText(entry2, "Some more text in the second entry");

  if Run(dialog) = GTK_RESPONSE_OK then
    result := [GetText(entry1), GetText(entry2)];
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
