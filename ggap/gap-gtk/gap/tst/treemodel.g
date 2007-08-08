#############################################################################
##
#W  treemodel.g                  ggap package                  Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation. See COPYING file that comes with this
##  distribution.
##
##  Use the following to run this file:
##  Read(Filename(DirectoriesLibrary("pkg/ggap/tst"), "treemodel.g"));
##

testlist := function()
  local store;

  store := GtkListStore(IsString);
  Append(store, "foo");
  Append(store, "bar");
  Assert(0, GetList(store) = ["foo", "bar"]);

  Display(GetItem(store, 1));
  Assert(0, GetItem(store, 1) = "foo");
  Display(GetItem(store, 2));
  Assert(0, GetItem(store, 2) = "bar");

  SetItem(store, 1, "baz");
  Display(GetItem(store, 1));
  Assert(0, GetItem(store, 1) = "baz");

  Insert(store, 1, "a");
  Display(GetItem(store, 1));
  Assert(0, GetItem(store, 1) = "a");
  Display(GetItem(store, 2));
  Assert(0, GetItem(store, 2) = "baz");

  Insert(store, 1, "b");
  Assert(0, GetItem(store, 1) = "b");
  Display(GetItem(store, 1));
  Assert(0, GetItem(store, 2) = "a");
  Display(GetItem(store, 2));

  Insert(store, 5, "c");
  Display(GetItem(store, 1));
  Assert(0, GetItem(store, 1) = "b");
  Display(GetItem(store, 5));
  Assert(0, GetItem(store, 5) = "c");

  while Length(store) <> 0 do
    Remove(store, Length(store));
  od;
  Display(GetList(store));
  Assert(0, GetList(store) = []);

  SetList(store, ["a", "b", "c"]);
  Display(GetList(store));
  Assert(0, GetList(store) = ["a", "b", "c"]);
  Clear(store);
  Display(GetList(store));
  Assert(0, GetList(store) = []);
end;

testcombo := function()
  local combo, store, items, i;

  Print("ComboBox\n");
  combo := GtkComboBox();
  store := GetModel(combo);

  items := ["a", "b", "c"];
  SetList(store, items);

  for i in [1..Length(items)] do
    SetActive(combo, i);
    Assert(0, GetActive(combo) = i);
    Assert(0, GetActiveText(combo) = items[GetActive(combo)]);
  od;
end;

testlist();
testcombo();
