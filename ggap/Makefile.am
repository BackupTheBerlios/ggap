SUBDIRS = moows .

EXTRA_PROGRAMS = ggap
bin_PROGRAMS =
noinst_PROGRAMS = testparser

if !MOO_OS_CYGWIN
bin_PROGRAMS += ggap
endif

AM_CFLAGS =				\
    -I$(top_srcdir)/moo-newdoc		\
    $(MOO_CFLAGS)			\
    $(MOO_DEBUG_CFLAGS)

testparser_LDADD = ../moo/moo/libmoo.la $(READLINE_LIBS)

ggap_LDADD = moows/libmoows.la ../moo-newdoc/mooui/libmooui.la ../moo/moo/libmoo.la $(ZLIB)
ggap_LDFLAGS = -export-dynamic -export-symbols-regex "^[^_].*"

CLEANFILES =
EXTRA_DIST =
BUILT_SOURCES =
nodist_ggap_SOURCES =

if MOO_OS_MINGW
ggap_LDFLAGS += -mwindows
rc_in_files = ggap_res.rc.in
nodist_ggap_SOURCES += ggap_res.res
include ../moo/mk/rc.mk
endif MOO_OS_MINGW

testparser_SOURCES =		\
	testparser.c		\
	gapparser.c		\
	gapparser.h		\
	gapparser-yacc.c	\
	gapparser-yacc.h

ggap_SOURCES =			\
	gap.c			\
	gap.h			\
	gapapp.c		\
	gapapp.h		\
	gapeditwindow.c		\
	gapeditwindow.h		\
	gapparser-priv.h	\
	gapparser-yacc.c	\
	gapparser-yacc.h	\
	gapparser.c		\
	gapparser.h		\
	gapprocess.c		\
	gapprocess.h		\
	gapterm.c		\
	gapterm.h		\
	gaptermwindow-ui.h	\
	gaptermwindow.c		\
	gaptermwindow.h		\
	gapworksheet.c		\
	gapworksheet.h		\
	gapwscompletion.c	\
	gapwscompletion.h	\
	gapwswindow-ui.h	\
	gapwswindow.c		\
	gapwswindow.h		\
	ggap-credits.h		\
	ggap-enums.c		\
	ggap-enums.h		\
	ggap-i18n.c		\
	ggap-i18n.h		\
	ggapfile.c		\
	ggapfile.h		\
	main.c

BUILT_SOURCES += ggap-credits.h
ggap-credits.h: ../THANKS $(MOO_XML2H)
	$(SHELL) $(MOO_XML2H) THANKS $(srcdir)/../THANKS > ggap-credits.h.tmp && \
	mv ggap-credits.h.tmp ggap-credits.h

enum_file = ggap-enums
enum_prefix = GAP
enum_headers =			\
	gap.h			\
	gapapp.h		\
	gapeditwindow.h		\
	gapparser.h		\
	gapprocess.h		\
	gapterm.h		\
	gaptermwindow.h		\
	gapworksheet.h		\
	gapwscompletion.h	\
	gapwswindow.h		\
	ggapfile.h
include ../../moo/mk/enums.mk

yacc_files = gapparser-yacc.y
include ../moo/mk/yacc.mk

ui_files = gapwswindow.xml gaptermwindow.xml
include ../moo/mk/ui.mk

glade_files = glade/gapprefs.glade
include ../moo/mk/glade.mk

mime_in_files = ggap.xml.in
include ../moo/mk/mime.mk

include gap/Makefile.incl


# include ../../moo/mk/marshals.mk
BUILT_SOURCES += marshals.c marshals.h
nodist_ggap_SOURCES += marshals.c marshals.h
CLEANFILES += marshals.c marshals.h
EXTRA_DIST += marshals.list
marshals.c: marshals.list
	glib-genmarshal --prefix=_ggap_marshal --header $(srcdir)/marshals.list > $@.tmp && \
	glib-genmarshal --prefix=_ggap_marshal --body $(srcdir)/marshals.list >> $@.tmp && \
	mv $@.tmp $@
marshals.h: marshals.list
	glib-genmarshal --prefix=_ggap_marshal --header $(srcdir)/marshals.list > $@.tmp && \
	mv $@.tmp $@
