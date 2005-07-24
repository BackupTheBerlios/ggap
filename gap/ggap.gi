#############################################################################
##
#W  ggap.gi                    ggap package                    Yevgen Muntyan
#W
#Y  Copyright (C) 2004-2005 by Yevgen Muntyan <muntyan@math.tamu.edu>
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##
##  See COPYING file that comes with this distribution.
##

Revision.ggap_gi :=
  "@(#)$Id: ggap.gi,v 1.1 2005/02/23 04:44:16 emuntyan Exp $";


#############################################################################
##
#F  GGAP_INIT(<pipe_name>)
##
InstallGlobalFunction("GGAP_INIT",
function(pipe)
    if GGAP_PARAMS.init then
        Error ("GGAP package is already initialized");
    fi;
    if not IsString (pipe) then
        Error ("argument is not a string");
    fi;
    GGAP_PARAMS.pipe_name := pipe;
    GGAP_PARAMS.pipe := OutputTextFile (pipe, true);
    GGAP_PARAMS.init := true;
end);


#############################################################################
##
#F  GGAP_SEND_STRING(<string1>, <string2>, ...)
##
InstallGlobalFunction("GGAP_SEND_STRING",
function(arg)
    local s;
    for s in arg do
        AppendTo (GGAP_PARAMS.pipe, s);
    od;
end);


#############################################################################
##
#F  GGAP_ESCAPE_STRING(string)
##
InstallGlobalFunction("GGAP_ESCAPE_STRING",
function(string)
    local c, result;
    result := "";
    for c in string do
        if c = '\\' then
            Append (result, "\\\\");
        elif c = '\"' then
            Append (result, "\\\"");
        else
            Add (result, c);
        fi;
    od;
    return result;
end);


#############################################################################
##
#F  GRead (<file>)
##
##  Tells ggap to execute <file> in python interpreter
##
InstallGlobalFunction("GRead",
function(filename)
    GGAP_SEND_STRING ("ggap.ExecFile(\'",
                      GGAP_ESCAPE_STRING (filename),
                      "\')\n");
end);


#############################################################################
##
#F  GStartFile (<file>)
##
##  Tells ggap to execute <file> as if it were double-clicked
##  Windows only
##
if ARCH_IS_WINDOWS() then
InstallGlobalFunction("GStartFile",
function(file)
    GGAP_SEND_STRING ("import os\nos.startfile(\'",
                      GGAP_ESCAPE_STRING (file),
                      "\')\n");
end);
fi;


#############################################################################
##
#F  GExec (<cmd>)
##
##  Tells ggap to execute <cmd> in a shell
##
InstallGlobalFunction("GExec",
function(cmd)
    GGAP_SEND_STRING ("import os\nos.system(\'",
                      GGAP_ESCAPE_STRING (cmd),
                      "\')\n");
end);


#E
