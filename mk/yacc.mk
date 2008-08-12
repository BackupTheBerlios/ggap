# -*- makefile -*-

# $(YACC_FILES) should be a list of .y files
# $(YACC_PP_FILES) should be a list of .y files
# $(yacc_sources) is defined to be a list of generated *-yacc.[ch] files

# bison_verbose = -v

$(srcdir)/%-yacc.h: %-yacc.y
	touch $(srcdir)/$@
$(srcdir)/%-yacc.c: %-yacc.y
	bison $(bison_verbose) -o $@ -d $<
$(srcdir)/%-yacc.cpp: %-yacc.y
	bison $(bison_verbose) -o $@ -d $< && \
	mv $(srcdir)/$*-yacc.hpp $(srcdir)/$*-yacc.h

yacc_sources =                              \
    $(patsubst %.y,%.h,$(YACC_FILES))       \
    $(patsubst %.y,%.c,$(YACC_FILES))       \
    $(patsubst %.y,%.h,$(YACC_PP_FILES))    \
    $(patsubst %.y,%.cpp,$(YACC_PP_FILES))

BUILT_SOURCES += $(yacc_sources)
EXTRA_DIST += $(yacc_files) $(YACC_PP_FILES)
