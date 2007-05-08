SUBDIRS = gap .

EXTRA_PROGRAMS = ggap pipehelper
bin_PROGRAMS =

if !MOO_OS_CYGWIN
bin_PROGRAMS += ggap
endif

if MOO_OS_MINGW
bin_PROGRAMS += pipehelper
endif

pipehelper_SOURCES = pipehelper.c pipehelper.h

AM_CFLAGS =				\
    -I$(top_builddir)			\
    -I$(top_builddir)/ggap		\
    -I$(top_srcdir)/ggap		\
    -I$(top_builddir)/moo/moo		\
    -I$(top_srcdir)/moo/moo		\
    $(MOO_DEBUG_CFLAGS)			\
    $(GTK_CFLAGS)			\
    -DGGAP_LOCALE_DIR=\"$(datadir)/locale\"

ggap_LDADD = $(top_builddir)/moo/moo/libmoo.la

ggap_LDFLAGS = -export-dynamic -export-symbols-regex "^[^_].*"

if MOO_OS_MINGW
ggap_LDFLAGS += -mwindows
endif MOO_OS_MINGW

ggap_SOURCES =				\
	ggap-ui.h			\
	ggap-mod.h			\
	gap.c				\
	gap.h				\
	ggap-i18n.h			\
	ggap.c				\
	gapapp.c			\
	gapapp.h			\
	gapdoc.c			\
	gapdoc.h			\
	gapdocwindow.c			\
	gapdocwindow.h			\
	gapeditwindow.c			\
	gapeditwindow.h			\
	gapoutput.c			\
	gapoutput.h			\
	gapsession.c			\
	gapsession.h			\
	gapterm.c			\
	gapterm.h			\
	gaptermwindow.c			\
	gaptermwindow.h			\
	gapprefs-glade.h		\
	gapdoc-glade.h			\
	ggap-credits.h

nodist_ggap_SOURCES = ggap_res.res
CLEANFILES = ggap_res.res

if MOO_OS_MINGW
ggap_res.res: ggap_res.rc
	$(WINDRES) -i ggap_res.rc --input-format=rc -o ggap_res.res -O coff
ggap_SOURCES += ggap_res.res
ggap_LDADD += ggap_res.res
else ! MOO_OS_MINGW
ggap_res.res: ggap_res.rc
	touch ggap_res.res
endif ! MOO_OS_MINGW

EXTRA_DIST =		\
    ggap_res.rc.in	\
    ggap-ui.xml		\
    ggap.opag		\
    glade/gapprefs.glade\
    glade/gapdoc.glade	\
    glade/usermenu.glade\
    python/ggap.py

BUILT_SOURCES = ggap-ui.h gapprefs-glade.h gapdoc-glade.h ggap.c ggap-mod.h ggap-credits.h
XML2H = $(top_srcdir)/xml2h.sh
PY2H = $(top_srcdir)/py2h.sh

ggap-credits.h: ../THANKS $(XML2H)
	$(SHELL) $(XML2H) THANKS $(srcdir)/../THANKS > ggap-credits.h
ggap-ui.h: ggap-ui.xml $(XML2H)
	sh $(XML2H) GGAP_UI $(srcdir)/ggap-ui.xml > ggap-ui.h

gapprefs-glade.h: glade/gapprefs.glade $(XML2H)
	sh $(XML2H) GAP_PREFS_GLADE_UI $(srcdir)/glade/gapprefs.glade > gapprefs-glade.h

gapdoc-glade.h: glade/gapdoc.glade $(XML2H)
	sh $(XML2H) GAP_DOC_GLADE_UI $(srcdir)/glade/gapdoc.glade > gapdoc-glade.h

ggap.c: ggap.opag
	opag -f _ggap_parse_options -O _ggap_opt_ -A _ggap_arg_ $(srcdir)/ggap.opag $(srcdir)/ggap.c

ggap-mod.h: $(srcdir)/python/ggap.py $(PY2H)
	$(SHELL) $(PY2H) GGAP_PY $(srcdir)/python/ggap.py > ggap-mod.h
