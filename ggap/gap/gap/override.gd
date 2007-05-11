###############################################################################
##
#O  GladeXML( <filename>[, <callbacks>] )
##
DeclareOperation("GladeXML", [IsString, IsRecord]);
DeclareOperation("GladeXML", [IsString]);



###############################################################################
##
#A  NColumns
##
DeclareAttribute("NColumns", IsGtkTreeModel);

###############################################################################
##
#O  Clear( <list> )
##
DeclareOperation("Clear", [IsGtkTreeModel]);

###############################################################################
##
#O  Insert( <list>, <pos>, <item> )
#O  Append( <list>, <item> )
#O  Remove( <list>, <pos> )
##
DeclareOperation("Insert", [IsGtkTreeModel, IsList, IsObject]);
DeclareOperation("Insert", [IsGtkTreeModel, IsInt, IsObject]);
DeclareOperation("Append", [IsGtkTreeModel, IsObject]);
DeclareOperation("Remove", [IsGtkTreeModel, IsList]);
DeclareOperation("Remove", [IsGtkTreeModel, IsPosInt]);

###############################################################################
##
#O  SetItem( <list>, <pos>, <item> )
#O  GetItem( <list>, <pos> )
##
DeclareOperation("SetItem", [IsGtkTreeModel, IsPosInt, IsObject]);
DeclareOperation("SetItem", [IsGtkTreeModel, IsList, IsObject]);
DeclareOperation("GetItem", [IsGtkTreeModel, IsPosInt]);
DeclareOperation("GetItem", [IsGtkTreeModel, IsList]);

###############################################################################
##
#F  GtkListStore()
##
DeclareGlobalFunction("GtkListStore");

###############################################################################
##
#O  SetList( <listobj>, <items> )
#O  GetList( <listobj> )
##
DeclareOperation("SetList", [IsGtkListStore, IsList]);
DeclareOperation("GetList", [IsGtkListStore]);
