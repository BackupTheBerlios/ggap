start := DirectoryCurrent();
dirs := DirectoriesLibrary("pkg/ggap/tst/");

for d in dirs do
  if IsDirectoryPath(d![1]) then
    start := d;
    break;
  fi;
od;

files := RunDialogFile(FILE_DIALOG_OPEN, start,
                       rec(title:="Pick Files",
                           multiple:=true));

if files <> fail then
  RunDialogMessage(DIALOG_INFO,
                   Concatenation("You have chosen\n",
                                 JoinStringsWithSeparator(files, "\n")));
fi;
