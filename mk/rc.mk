# -*- makefile -*-

# $(RC_IN_FILES) should be a list of input *.rc.in files

if MOO_OS_MINGW

%.res: %.rc.in $(top_builddir)/config.status
	cd $(top_builddir) && \
	$(SHELL) ./config.status --file=$(subdir)/$*.rc && \
	cd $(subdir) && \
	$(WINDRES) -i $*.rc --input-format=rc -o $@ -O coff && \
	rm $*.rc

res_files = $(RC_IN_FILES:.rc.in=.res)

CLEANFILES += $(res_files)
BUILT_SOURCES += $(res_files)
@MODULE@_LDADD += $(res_files)

endif

EXTRA_DIST += $(RC_IN_FILES)
