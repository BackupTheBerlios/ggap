noinst_LTLIBRARIES = libmooui.la

libmooui_la_SOURCES =		\
	mdaccels.h		\
	mdasyncop.c		\
	mdasyncop.h		\
	mddialogs.c		\
	mddialogs.h		\
	mddocument.c		\
	mddocument.h		\
	mddocument-private.h	\
	mdenums.c		\
	mdenums.h		\
	mdfileops.c		\
	mdfileops.h		\
	mdmanager.c		\
	mdmanager.h		\
	mdmanager-private.h	\
	mdprefs.h		\
	mdtypes.h		\
	mdutils.c		\
	mdutils.h		\
	mdwindow.c		\
	mdwindow.h		\
	mdwindow-private.h

%-private.lo: %.lo
	@true

AM_CFLAGS =			\
	-I$(top_builddir)	\
	-I..			\
	-I$(srcdir)/..		\
	-I$(top_srcdir)/doc	\
	-I$(top_builddir)/doc	\
	$(MOO_CFLAGS)		\
	$(MOO_DEBUG_CFLAGS)	\
	$(GTK_CFLAGS)

enum_file = mdenums
enum_prefix = MD
enum_headers =			\
	mdasyncop.h		\
	mddialogs.h		\
	mddocument.h		\
	mdfileops.h		\
	mdmanager.h		\
	mdtypes.h		\
	mdwindow.h

glade_files =			\
	glade/mdopenuri.glade	\
	glade/mdsavemult.glade	\
	glade/mdpasswddlg.glade

BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =
include ../../moo/mk/enums.mk
include ../../moo/mk/glade.mk

# include ../../moo/mk/marshals.mk
BUILT_SOURCES += marshals.c marshals.h
nodist_libmooui_la_SOURCES = marshals.c marshals.h
CLEANFILES += marshals.c marshals.h
EXTRA_DIST += marshals.list
marshals.c: marshals.list
	glib-genmarshal --prefix=_moo_ui_marshal --header $(srcdir)/marshals.list > $@.tmp && \
	glib-genmarshal --prefix=_moo_ui_marshal --body $(srcdir)/marshals.list >> $@.tmp && \
	mv $@.tmp $@
marshals.h: marshals.list
	glib-genmarshal --prefix=_moo_ui_marshal --header $(srcdir)/marshals.list > $@.tmp && \
	mv $@.tmp $@
