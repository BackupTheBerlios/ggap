#
# @(#)$Id: Makefile.am,v 1.13 2005/05/29 06:47:05 emuntyan Exp $
#

SUBDIRS = glade .

DEFAULT_INCLUDES =
AM_CFLAGS =                     \
    -I$(top_builddir)           \
    -I$(top_builddir)/src       \
    -I$(top_builddir)/src/moo   \
    -I$(top_srcdir)/src         \
    $(MOOEDIT_CFLAGS)           \
    $(PYTHON_INCLUDES)          \
    $(PYGTK_CFLAGS)             \
    $(CFLAGS)                   \
    $(M_CFLAGS)
AM_CXXFLAGS =                   \
    -I$(top_builddir)           \
    -I$(top_builddir)/src       \
    -I$(top_builddir)/src/moo   \
    -I$(top_srcdir)/src         \
    $(MOOEDIT_CFLAGS)           \
    $(PYTHON_INCLUDES)          \
    $(PYGTK_CFLAGS)             \
    $(CXXFLAGS)                 \
    $(M_CXXFLAGS)

noinst_LTLIBRARIES = libgap.la

libgap_la_SOURCES =     \
    gap.c               \
    gap.h               \
    gapapp.c            \
    gapapp.h            \
    gapoutchannel.c     \
    gapoutchannel.h     \
    gapprefs.h          \
    gapprefs-glade.c    \
    gapprefsdialog.c    \
    gapprefsdialog.h    \
    gaptermwindow.c     \
    gaptermwindow.h     \
    gapui.h

if USE_PYGTK
pygtk_sources = gapapp-pygtk.c
nodist_libgap_la_SOURCES = $(pygtk_sources)
endif

EXTRA_DIST = gapui.xml gapapp-pygtk.c gapapp-pygtk.defs gapapp-pygtk.override
BUILT_SOURCES = gapui.h $(pygtk_sources)

XML2H = $(top_srcdir)/$(MOO_SRC_PREFIX)/mooutils/xml2h.sh
gapui.h: $(srcdir)/gapui.xml $(XML2H)
	sh $(XML2H) GAP_UI $(srcdir)/gapui.xml > gapui.h

GLADE = $(srcdir)/glade
SUPPORT = $(top_srcdir)/$(MOO_SRC_PREFIX)/mooutils/glade/support.sh

gapprefs-glade.c: $(GLADE)/gapprefs.c $(GLADE)/gapprefs.sed $(SUPPORT)
	sh $(SUPPORT) $(GLADE)/gapprefs.c | \
		sed -f $(GLADE)/gapprefs.sed > gapprefs-glade.c

if USE_PYGTK
gapapp-pygtk.c: $(srcdir)/gapapp-pygtk.defs $(srcdir)/gapapp-pygtk.override
	pygtk-codegen-2.0 --prefix gap_app \
	--register $(PYGTK_DEFS_DIR)/gtk-types.defs \
	--override $(srcdir)/gapapp-pygtk.override \
	$(srcdir)/gapapp-pygtk.defs > $(srcdir)/gapapp-pygtk.c
endif
