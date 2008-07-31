$GGAP_PAGER := function(lines)
  local   formatted,  linepos,  size,  wd,  pl,  count,  i,
          len,  char;

  formatted := false;
  linepos := 1;

  if IsRecord(lines) then
    if IsBound(lines.formatted) then
      formatted := lines.formatted;
    fi;
    if IsBound(lines.start) then
      linepos := lines.start;
    fi;
    lines := lines.lines;
  fi;

  if IsString(lines) then
    lines := SplitString(lines, "\n", "");
  fi;

  size := SizeScreen();
  wd := QuoInt(size[1]+2, 2);
  # really print line without breaking it
  pl := function(l)
    local   r;
    r := 1;
    while r*wd<=Length(l) do
      Print(l{[(r-1)*wd+1..r*wd]}, "\c");
      r := r+1;
    od;
    if (r-1)*wd < Length(l) then
      Print(l{[(r-1)*wd+1..Length(l)]});
    fi;
    Print("\n");
  end;

  if not formatted then
    # cope with overfull lines
    count:=1;
    while count<=Length(lines) do
      if Length(lines[count])>size[1]-2 then
        # find the last blank before this position
        i:=size[1]-2;
        while i>0 and lines[count][i]<>' ' do
          i:=i-1;
        od;
        if i>0 then
          if not IsBound(lines[count+1]) then
            lines[count+1]:="";
          fi;
          lines[count+1]:=Concatenation(
             lines[count]{[i+1..Length(lines[count])]}," ", lines[count+1]);
          lines[count]:=lines[count]{[1..i-1]};
        fi;
      fi;
      count:=count+1;
    od;
  fi;

  for i in [linepos..Length(lines)] do
    pl(lines[i]);
  od;
end;

# $GGAP_SIZE_SCREEN := function(arg)
# end;

Add($GGAP_DATA.api, "$GGAP_PAGER");
# Add($GGAP_DATA.api, "$GGAP_SIZE_SCREEN");
