noinst_PROGRAMS = testws
noinst_LTLIBRARIES = libmoows.la

testws_LDADD = libmoows.la $(top_builddir)/moo/moo/libmoo.la $(MOO_OBJC_LIBS)

testws_SOURCES =		\
	testws.c

libmoows_la_SOURCES =		\
	mooworksheet.c		\
	mooworksheet.h		\
	moowspromptblock.c	\
	moowspromptblock.h	\
	moowstextblock.c	\
	moowstextblock.h	\
	moowsblock.c		\
	moowsblock.h		\
	moowsview.c		\
	moowsview.h

if !GLIB_2_14
regex_cflags = -I$(srcdir)/../moo/moo/mooutils/newgtk/glib-2.14
endif

AM_CFLAGS =				\
    -I$(top_builddir)			\
    -I$(top_builddir)/ggap		\
    -I$(top_srcdir)/ggap		\
    -I$(top_builddir)/moo/moo		\
    -I$(top_srcdir)/moo/moo		\
    $(regex_cflags)			\
    $(MOO_DEBUG_CFLAGS)			\
    $(GTK_CFLAGS)			\
    -DGGAP_LOCALE_DIR=\"$(datadir)/locale\"
AM_OBJCFLAGS = $(AM_CFLAGS) $(MOO_OBJCFLAGS)
