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
