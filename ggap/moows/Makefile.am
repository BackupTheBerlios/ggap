noinst_PROGRAMS = testws
noinst_LTLIBRARIES = libmoows.la

testws_LDADD = libmoows.la $(MOO_LIBS) $(MOO_OBJC_LIBS)

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
	moowsbuffer.c		\
	moowsbuffer.h		\
	moowsview.c		\
	moowsview.h

if !GLIB_2_14
regex_cflags = -I$(srcdir)/../moo/moo/mooutils/newgtk/glib-2.14
endif

AM_CFLAGS =				\
    -I$(top_builddir)			\
    -I$(top_builddir)/ggap		\
    -I$(top_srcdir)/ggap		\
    $(regex_cflags)			\
    $(MOO_CFLAGS)			\
    $(MOO_DEBUG_CFLAGS)			\
    $(GTK_CFLAGS)			\
    -DDISABLE_OLD_STUFF
AM_OBJCFLAGS = $(AM_CFLAGS) $(MOO_OBJCFLAGS)
