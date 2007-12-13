noinst_LTLIBRARIES = libmooui.la
noinst_PROGRAMS = testui

libmooui_la_SOURCES =			\
	mddialogs.c			\
	mddialogs.h			\
	mddocument.c			\
	mddocument.h			\
	mddocument-private.h		\
	mdwindow.c			\
	mdwindow.h			\
	mdwindow-private.h		\
	mdmanager.c			\
	mdmanager.h

mddocument-private.lo: mddocument.lo
mdwindow-private.lo: mdwindow.lo
mdmanager-private.lo: mdmanager.lo

testui_LDADD = libmooui.la $(MOO_LIBS)

AM_CFLAGS =				\
    -I$(top_builddir)			\
    -I$(top_builddir)/ggap		\
    -I$(top_srcdir)/ggap		\
    $(MOO_CFLAGS)			\
    $(MOO_DEBUG_CFLAGS)			\
    $(GTK_CFLAGS)			\
    -DGGAP_LOCALE_DIR=\"$(datadir)/locale\"
AM_OBJCFLAGS = $(AM_CFLAGS) $(MOO_OBJCFLAGS)

EXTRA_DIST = marshals.list

BUILT_SOURCES =				\
	marshals.c			\
	marshals.h
nodist_libmooui_la_SOURCES =		\
	marshals.c			\
	marshals.h
CLEANFILES =				\
	marshals.c			\
	marshals.h

marshals.c: marshals.list
	echo '#include "marshals.h"' > $@.tmp &&		\
	glib-genmarshal --prefix=_moo_ui_marshal --body		\
	       $(srcdir)/marshals.list >> $@.tmp &&		\
	mv $@.tmp $@
marshals.h: marshals.list
	glib-genmarshal --prefix=_moo_ui_marshal --header	\
	       $(srcdir)/marshals.list > $@.tmp &&		\
	mv $@.tmp $@
