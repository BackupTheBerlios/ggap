SUBDIRS = gap .

EXTRA_PROGRAMS = ggap pipehelper
bin_PROGRAMS =

if !MOO_OS_CYGWIN
bin_PROGRAMS += ggap
endif

if MOO_OS_MINGW
bin_PROGRAMS += pipehelper
endif

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
	gapview.c			\
	gapview.h			\
	gapeditwindow.c			\
	gapeditwindow.h			\
	gapwindow.c			\
	gapwindow.h			\
	gapworksheet.c			\
	gapworksheet.h			\
	mooworksheet.c			\
	mooworksheet.h			\
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
    glade/gapprefs.glade\
    glade/usermenu.glade\
    python/ggap.py	\
    python/gap_canvas.py\
    python/gap_cb.py	\
    python/gap_types.py	\
    python/gap.py

BUILT_SOURCES =		\
	ggap-ui.h	\
	gapprefs-glade.h\
	ggap.c		\
	ggap-mod.h	\
	gap-canvas-mod.h\
	gap-cb-mod.h	\
	gap-types-mod.h	\
	gap-mod.h	\
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

ggap-mod.h: $(srcdir)/python/ggap.py $(PY2H)
	$(SHELL) $(PY2H) GGAP_PY $(srcdir)/python/ggap.py > ggap-mod.h
gap-mod.h: $(srcdir)/python/gap.py $(PY2H)
	$(SHELL) $(PY2H) GAP_PY $(srcdir)/python/gap.py > gap-mod.h
gap-cb-mod.h: $(srcdir)/python/gap_cb.py $(PY2H)
	$(SHELL) $(PY2H) GAP_CB_PY $(srcdir)/python/gap_cb.py > gap-cb-mod.h
gap-canvas-mod.h: $(srcdir)/python/gap_canvas.py $(PY2H)
	$(SHELL) $(PY2H) GAP_CANVAS_PY $(srcdir)/python/gap_canvas.py > gap-canvas-mod.h
gap-types-mod.h: $(srcdir)/python/gap_types.py $(PY2H)
	$(SHELL) $(PY2H) GAP_TYPES_PY $(srcdir)/python/gap_types.py > gap-types-mod.h
