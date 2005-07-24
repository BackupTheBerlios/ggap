#
# @(#)$Id: Makefile.am,v 1.3 2005/05/29 03:35:04 emuntyan Exp $
#

SUBDIRS = glade .

DEFAULT_INCLUDES =
AM_CFLAGS =                 \
    -I$(top_builddir)       \
    -I$(top_builddir)/src   \
    -I$(top_srcdir)/src     \
    $(MOO_CFLAGS)           \
    $(PYTHON_INCLUDES)      \
    $(PYGTK_CFLAGS)         \
    $(CFLAGS)               \
    $(M_CFLAGS)
AM_CXXFLAGS =               \
    -I$(top_builddir)       \
    -I$(top_builddir)/src   \
    -I$(top_srcdir)/src     \
    $(MOO_CFLAGS)           \
    $(PYTHON_INCLUDES)      \
    $(PYGTK_CFLAGS)         \
    $(CXXFLAGS)             \
    $(M_CXXFLAGS)

noinst_LTLIBRARIES = libsc3.la

libsc3_la_SOURCES =     \
    sc3app.cpp          \
    sc3app.h            \
    sc3prefs.h          \
    sc3prefs-glade.c    \
    sc3prefsdialog.c    \
    sc3prefsdialog.h    \
    sc3termwindow.c     \
    sc3termwindow.h     \
    sc3ui.h

if USE_PYGTK
libsc3_la_SOURCES += sc3app-pygtk.c
endif

EXTRA_DIST = sc3ui.xml sc3app-pygtk.defs sc3app-pygtk.override
BUILT_SOURCES = sc3ui.h sc3app-pygtk.c

XML2H = $(top_srcdir)/$(MOO_SRC_PREFIX)/mooutils/xml2h.sh
sc3ui.h: $(srcdir)/sc3ui.xml $(XML2H)
	sh $(XML2H) SC3_UI $(srcdir)/sc3ui.xml > sc3ui.h

GLADE = $(srcdir)/glade
SUPPORT = $(top_srcdir)/$(MOO_SRC_PREFIX)/mooutils/glade/support.sh

sc3prefs-glade.c: $(GLADE)/sc3prefs.c $(GLADE)/sc3prefs.sed $(SUPPORT)
	sh $(SUPPORT) $(GLADE)/sc3prefs.c | \
		sed -f $(GLADE)/sc3prefs.sed > sc3prefs-glade.c

sc3app-pygtk.c: $(srcdir)/sc3app-pygtk.defs $(srcdir)/sc3app-pygtk.override
	pygtk-codegen-2.0 --prefix sc3_app \
	--register $(PYGTK_DEFS_DIR)/gtk-types.defs \
	--override $(srcdir)/sc3app-pygtk.override \
	$(srcdir)/sc3app-pygtk.defs > $(srcdir)/sc3app-pygtk.c
