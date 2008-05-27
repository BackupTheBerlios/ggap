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

if MOO_OS_MINGW
ggap_LDFLAGS += -mwindows
rc_in_files = ggap_res.rc.in
nodist_ggap_SOURCES = ggap_res.res
include ../moo/mk/rc.mk
endif MOO_OS_MINGW

testparser_SOURCES =		\
	testparser.c		\
	gapparser.c		\
	gapparser.h		\
	gapparser-yacc.c	\
	gapparser-yacc.h

ggap_SOURCES =			\
	gapworksheet.c		\
	gapworksheet.h		\
	gapwscompletion.c	\
	gapwscompletion.h	\
	gapwswindow.c		\
	gapwswindow.h		\
	gapeditwindow.c		\
	gapeditwindow.h		\
	gapwswindow-ui.h	\
	gaptermwindow-ui.h	\
	gap.c			\
	gap.h			\
	ggap-i18n.h		\
	ggap-i18n.c		\
	main.c			\
	ggapfile.h		\
	ggapfile.c		\
	gapparser.c		\
	gapparser.h		\
	gapparser-priv.h	\
	gapparser-yacc.c	\
	gapparser-yacc.h	\
	gapterm.c		\
	gapterm.h		\
	gaptermwindow.c		\
	gaptermwindow.h		\
	gapapp.c		\
	gapapp.h		\
	gapprocess.c		\
	gapprocess.h		\
	ggap-enums.c		\
	ggap-enums.h		\
	ggap-credits.h

BUILT_SOURCES += ggap-credits.h
ggap-credits.h: ../THANKS $(MOO_XML2H)
	$(SHELL) $(MOO_XML2H) THANKS $(srcdir)/../THANKS > ggap-credits.h.tmp && \
	mv ggap-credits.h.tmp ggap-credits.h

enum_file = ggap-enums
enum_prefix = GAP
enum_headers =			\
	gapworksheet.h		\
	gapwscompletion.h	\
	gapwswindow.h		\
	gapeditwindow.h		\
	gap.h			\
	ggapfile.h		\
	gapparser.h		\
	gapterm.h		\
	gaptermwindow.h		\
	gapapp.h		\
	gapprocess.h
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
