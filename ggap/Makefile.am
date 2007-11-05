SUBDIRS = gap moows .

EXTRA_PROGRAMS = ggap pipehelper
bin_PROGRAMS =
noinst_PROGRAMS = testparser

if !MOO_OS_CYGWIN
bin_PROGRAMS += ggap
endif

if MOO_OS_MINGW
bin_PROGRAMS += pipehelper
endif

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

testparser_LDADD = $(top_builddir)/moo/moo/libmoo.la $(MOO_OBJC_LIBS) $(READLINE_LIBS)

ggap_LDADD = moows/libmoows.la $(top_builddir)/moo/moo/libmoo.la $(MOO_OBJC_LIBS)
ggap_LDFLAGS = -export-dynamic -export-symbols-regex "^[^_].*"

if MOO_OS_MINGW
ggap_LDFLAGS += -mwindows
endif MOO_OS_MINGW

testparser_SOURCES =			\
	testparser.c			\
	gapparser.c			\
	gapparser.h			\
	gapparser-yacc.c		\
	gapparser-yacc.h

ggap_SOURCES =				\
	ggap-ui.h			\
	gap-mod.h			\
	gap-cb-mod.h			\
	gap-canvas-mod.h		\
	gap-types-mod.h			\
	ggap-mod.h			\
	gap.c				\
	gap.h				\
	ggap-i18n.h			\
	ggap-i18n.c			\
	ggap.c				\
	gapapp.c			\
	gapapp.h			\
	gapparser.c			\
	gapparser.h			\
	gapparser-priv.h		\
	gapparser-yacc.c		\
	gapparser-yacc.h		\
	gapview.c			\
	gapview.h			\
	gapeditwindow.c			\
	gapeditwindow.h			\
	gapwindow.c			\
	gapwindow.h			\
	gapworksheet.c			\
	gapworksheet.h			\
	gapwswindow.c			\
	gapwswindow.h			\
	gapoutput.c			\
	gapoutput.h			\
	gapsession.c			\
	gapsession.h			\
	gapterm.c			\
	gapterm.h			\
	gaptermwindow.c			\
	gaptermwindow.h			\
	gapprefs-glade.h		\
	ggap-credits.h

gapparser-yacc.h: gapparser-yacc.c
	touch $(srcdir)/gapparser-yacc.h
gapparser-yacc.c: gapparser-yacc.y
	bison -v -o $(srcdir)/gapparser-yacc.c -d $(srcdir)/gapparser-yacc.y

nodist_ggap_SOURCES = ggap_res.res
nodist_pipehelper_SOURCES = pipehelper_res.res
CLEANFILES = ggap_res.res pipehelper_res.res

if MOO_OS_MINGW
ggap_res.res: ggap_res.rc
	$(WINDRES) -i ggap_res.rc --input-format=rc -o ggap_res.res -O coff
pipehelper_res.res: pipehelper_res.rc
	$(WINDRES) -i pipehelper_res.rc --input-format=rc -o pipehelper_res.res -O coff
ggap_SOURCES += ggap_res.res
ggap_LDADD += ggap_res.res
pipehelper_SOURCES = pipehelper.c pipehelper.h pipehelper_res.res
pipehelper_LDADD = pipehelper_res.res
else ! MOO_OS_MINGW
ggap_res.res:
	echo dummy > ggap_res.res
pipehelper_res.res:
	echo dummy > pipehelper_res.res
endif ! MOO_OS_MINGW

EXTRA_DIST =		\
    ggap_res.rc.in	\
    pipehelper_res.rc.in\
    ggap-ui.xml		\
    ggap.opag		\
    gapparser-yacc.y	\
    glade/gapprefs.glade\
    glade/usermenu.glade

BUILT_SOURCES =		\
	gapparser-yacc.c\
	gapparser-yacc.h\
	ggap-ui.h	\
	gapprefs-glade.h\
	ggap.c		\
	ggap-credits.h

XML2H = $(top_srcdir)/moo/moo/mooutils/xml2h.sh
PY2H = $(top_srcdir)/moo/moo/mooutils/py2h.sh

ggap-credits.h: ../THANKS $(XML2H)
	$(SHELL) $(XML2H) THANKS $(srcdir)/../THANKS > ggap-credits.h.tmp && \
	mv ggap-credits.h.tmp ggap-credits.h
ggap-ui.h: ggap-ui.xml $(XML2H)
	sh $(XML2H) GGAP_UI $(srcdir)/ggap-ui.xml > ggap-ui.h.tmp && \
	mv ggap-ui.h.tmp ggap-ui.h

gapprefs-glade.h: glade/gapprefs.glade $(XML2H)
	sh $(XML2H) GAP_PREFS_GLADE_UI $(srcdir)/glade/gapprefs.glade > gapprefs-glade.h

ggap.c: ggap.opag
	opag -f _ggap_parse_options -O _ggap_opt_ -A _ggap_arg_ $(srcdir)/ggap.opag $(srcdir)/ggap.c
