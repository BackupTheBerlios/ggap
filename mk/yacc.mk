# -*- makefile -*-

# $(yacc_files) should be a list of .y files
# $(yacc_pp_files) should be a list of .y files
# $(yacc_sources) is defined to be a list of generated *-yacc.[ch] files

# $(lex_files) should be a list of .y files
# $(lex_pp_files) should be a list of .y files
# $(lex_sources) is defined to be a list of generated *-lex.c files

# bison_verbose = -v
lex_debug = -d

$(srcdir)/%-yacc.h: %-yacc.y
	touch $(srcdir)/$@
$(srcdir)/%-yacc.c: %-yacc.y
	bison $(bison_verbose) -o $@ -d $<
$(srcdir)/%-yacc.cpp: %-yacc.y
	bison $(bison_verbose) -o $@ -d $< && \
	mv $(srcdir)/$*-yacc.hpp $(srcdir)/$*-yacc.h

$(srcdir)/%-lex.c: %-lex.l
	flex $(lex_debug) -o$@ $<
$(srcdir)/%-lex.cpp: %-lex.l
	flex $(lex_debug) -o$@ $<

yacc_sources =                              \
    $(patsubst %.y,%.h,$(yacc_files))       \
    $(patsubst %.y,%.c,$(yacc_files))       \
    $(patsubst %.y,%.h,$(yacc_pp_files))    \
    $(patsubst %.y,%.cpp,$(yacc_pp_files))

lex_sources =				    \
    $(patsubst %.l,%.c,$(lex_files))	    \
    $(patsubst %.l,%.cpp,$(lex_pp_files))

BUILT_SOURCES += $(yacc_sources) $(lex_sources)
EXTRA_DIST += $(yacc_files) $(yacc_pp_files) $(lex_files) $(lex_pp_files)
