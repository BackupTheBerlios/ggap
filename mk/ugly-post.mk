# -*- makefile -*-

UGLY=$(top_srcdir)/mk/ugly

$(srcdir)/Makefile.am: $(srcdir)/Makefile.ug $(UGLY)
	@echo 'cd $(srcdir) && $(UGLY) $(top_srcdir) $(subdir)'; \
	cd $(srcdir) && $(UGLY) $(top_srcdir) $(subdir) && exit 0; \
	exit 1;

EXTRA_DIST +=	\
    Makefile.ug
