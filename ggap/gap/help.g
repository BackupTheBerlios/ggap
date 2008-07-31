#
# HELP() from lib/helpbase.gi
#

$GGAP_HELP_DATA := rec(
  known_books := [],
  last_topics := [],
);
Add($GGAP_DATA.api, "$GGAP_HELP_DATA");

$GGAP_SEND_BOOK_INFO := function(book)
end;
Add($GGAP_DATA.api, "$GGAP_SEND_BOOK_INFO");

$GGAP_HELP_SIMPLE_STRING := function(str)
  local trans, c, i;

  trans := "";
  i := 1;
  while i <= Length(str) do
    if str[i] in "\003\007" then
      i := i + 1;
      continue;
    fi;
    if str[i] = '\033' then
      i := i + 1;
      if i + 2 <= Length(str) and str[i] = '[' and str[i+2] = 'm' and str[i+1] in "0123456789" then
        i := i + 3;
      elif i + 3 <= Length(str) and str[i] = '[' and str[i+3] = 'm' and str[i+1] in "0123456789" and str[i+2] in "0123456789" then
        i := i + 4;
      fi;
      continue;
    fi;
    Add(trans, str[i]);
    i := i + 1;
  od;

  NormalizeWhitespace(trans);
  return trans;
end;
Add($GGAP_DATA.api, "$GGAP_HELP_SIMPLE_STRING");

$GGAP_HELP_MAKE_URI := function(str)
  if IsString(str) then
    # let ggap parse it
    return Concatenation("gfile:", str);
  else
    return str;
  fi;
end;
Add($GGAP_DATA.api, "$GGAP_HELP_MAKE_URI");

$GGAP_HELP_PRINT_MATCHES := function(entries)
  local e, cnt, len;

  if IsEmpty(entries) then
    return;
  fi;

  if Length(entries) = 1 then
    $GGAP_SEND_DATA("markup:Help: opening <a href=\"", entries[1][2], "\">", entries[1][1], "</a>\n");
    $GGAP_SEND_DATA("script:openUrl('", entries[1][2], "')");
    return;
  fi;

  if Length(entries) > 99 then
    Print("Help: too many entries match this topic, displaying first 100 (type ?2 to get match [2])\n");
    entries := entries{[1..99]};
  else
    Print("Help: several entries match this topic (type ?2 to get match [2])\n");
  fi;

  cnt := 1;
  len := LogInt(Length(entries), 10) + 1;
  for e in entries do
    $GGAP_SEND_DATA("markup:<a href=\"", e[2], "\">[", String(cnt, len), "] ", e[1], "</a>\n");
    cnt := cnt + 1;
  od;
end;
Add($GGAP_DATA.api, "$GGAP_HELP_PRINT_MATCHES");

$GGAP_HELP_DO_SHOW_MATCHES := function(matches)
  local m, all_books, book, no, e, data, topic, entries, cnt, len;

  matches := List(matches, x -> [HELP_BOOK_INFO(x[1]), x[2]]);
  all_books := AsSet(List(matches, x -> x[1]));

  for book in all_books do
    if not book in $GGAP_HELP_DATA.known_books then
      Add($GGAP_HELP_DATA.known_books, book);
      $GGAP_SEND_BOOK_INFO(book);
    fi;
  od;

  entries := [];
  for m in matches do
    book := m[1];
    no := m[2];
    e := book.entries[no];
    data := HELP_BOOK_HANDLER.(book.handler).HelpData(book, no, "url");
    if data <> fail then
      data := $GGAP_HELP_MAKE_URI(data);
    fi;
    if data <> fail then
      topic := $GGAP_HELP_SIMPLE_STRING(Concatenation(book.bookname, ": ", e[1]));
      Add(entries, [topic, data]);
    fi;
  od;

  $GGAP_HELP_DATA.last_topics := entries;

  if IsEmpty(entries) then
    return false;
  fi;

  $GGAP_HELP_PRINT_MATCHES(entries);
  return true;
end;
Add($GGAP_DATA.api, "$GGAP_HELP_DO_SHOW_MATCHES");


$GGAP_HELP_SHOW_MATCHES :=
function(books, topic, frombegin)
  local exact, match, x, lines;

  # first get lists of exact and other matches
  x := HELP_GET_MATCHES(books, topic, frombegin);
  exact := x[1];
  match := x[2];

  # no topic found
  if 0 = Length(match) and 0 = Length(exact)  then
    Print("Help: no matching entry found\n");
    return false;
  fi;

  # one exact or together one topic found
  if 1 = Length(exact) or (0 = Length(exact) and 1 = Length(match)) then
    if Length(exact) = 0 then exact := match; fi;
    if $GGAP_HELP_DO_SHOW_MATCHES(exact) then
      return true;
    fi;
  fi;

  match := Concatenation(exact, match);
  if not $GGAP_HELP_DO_SHOW_MATCHES(match) then
    Print("Help: no matching entry found\n");
    return false;
  else
    return true;
  fi;
end;
Add($GGAP_DATA.api, "$GGAP_HELP_SHOW_MATCHES");


$GGAP_HELP := function(str)
  local origstr, p, book, books, b;

  origstr := ShallowCopy(str);

  # extract the book
  p := Position(str, ':');
  if p <> fail then
    book := str{[1..p-1]};
    str  := str{[p+1..Length(str)]};
  else
    book := "";
  fi;

  # normalizing for search
  book := SIMPLE_STRING(book);
  str := SIMPLE_STRING(str);

  # we check if `book' MATCH_BEGINs some of the available books
  books := Filtered(HELP_KNOWN_BOOKS[1], bn -> MATCH_BEGIN(bn, book));
  if Length(book) > 0 and Length(books) = 0 then
    Print("Help: None of the available books matches (try: '?books').\n");
    return;
  fi;

  # if the topic is empty show the tutorial
  if book = "" and str = "" then
    $GGAP_HELP("Tutorial: Help");
    return;

  elif not IsEmpty(str) and Int(str) <> fail then
    p := Int(str);
    if p < 1 or p > Length($GGAP_HELP_DATA.last_topics) then
      Print("Help: No topic with number ", p, ".\n");
    else
      $GGAP_HELP_PRINT_MATCHES($GGAP_HELP_DATA.last_topics{[p]});
    fi;
    return;

  # XXX
#   # if the subject is 'Welcome to GAP' display a welcome message
#   elif book = "" and str = "welcome to gap"  then
#     add(books, "Welcome to GAP");

  # XXX
#   # if the topic is 'books' display the table of books
#   elif book = "" and str = "books"  then
#     add(books, "books");

  # XXX
#   # if the topic is 'chapters' display the table of chapters
#   elif str = "chapters" or book <> "" and str = "" then
#     add(books, "chapters");

  # XXX
#   # if the topic is 'sections' display the table of sections
#   elif str = "sections" then
#     add(books, "sections");

  # if the topic is '?<string>' search the index for any entries for
  # which <string> is a substring (as opposed to an abbreviation)
  elif Length(str) > 0 and str[1] = '?' then
    str := str{[2..Length(str)]};
    NormalizeWhitespace(str);
    $GGAP_HELP_SHOW_MATCHES(books, str, false);

  # search for this topic
  elif $GGAP_HELP_SHOW_MATCHES(books, str, true) then
    ;
  elif origstr in NAMES_SYSTEM_GVARS then
      Print("Help: '", origstr, "' is currently undocumented.\n",
            "      For details, try ?Undocumented Variables\n" );
  elif book = "" and ForAny(HELP_KNOWN_BOOKS[1], bk -> MATCH_BEGIN(bk, str)) then
      Print("Help: Are you looking for a certain book? (Trying '?", origstr,
            ":' ...\n");
      $GGAP_HELP(Concatenation(origstr, ":"));
  fi;
end;
Add($GGAP_DATA.api, "$GGAP_HELP");
