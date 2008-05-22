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
include ../../moo/mk/marshals.mk
include ../../moo/mk/glade.mk
