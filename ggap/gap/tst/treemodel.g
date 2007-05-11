#############################################################################
##
##  treemodel.g
##
##  Use the following to run it:
##  Read(Filename(DirectoriesLibrary("pkg/ggap/tst"), "treemodel.g"));
##

testlist := function()
  local store;

  store := GtkListStore(IsString);
  Append(store, "foo");
  Append(store, "bar");

  Display(GetItem(store, 1));
  Display(GetItem(store, 2));

  SetItem(store, 1, "baz");
  Display(GetItem(store, 1));

  Insert(store, 1, "a");
  Display(GetItem(store, 1));
  Display(GetItem(store, 2));

  Insert(store, 1, "b");
  Display(GetItem(store, 1));
  Display(GetItem(store, 2));

  Insert(store, 5, "c");
  Display(GetItem(store, 1));
  Display(GetItem(store, 5));

  while Length(store) <> 0 do
    Remove(store, Length(store));
  od;
  Display(GetList(store));

  SetList(store, ["a", "b", "c"]);
  Display(GetList(store));
  Clear(store);
  Display(GetList(store));
end;

testlist();
