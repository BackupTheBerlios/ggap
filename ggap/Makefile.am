#
# @(#)$Id: Makefile.am,v 1.69 2005/06/06 14:45:54 emuntyan Exp $
#

EXTRA_PROGRAMS = ggap termhelper

if !MOO_OS_CYGWIN
bin_PROGRAMS = ggap
endif

DEFAULT_INCLUDES =
AM_CFLAGS =                     \
    -I$(top_builddir)           \
    -I$(top_builddir)/ggap      \
    -I$(top_srcdir)/ggap        \
    -I$(top_builddir)/ggap/moo  \
    -I$(top_srcdir)/ggap/moo    \
    $(PYTHON_INCLUDES)          \
    $(MOO_CFLAGS)               \
    $(PYGTK_CFLAGS)             \
    $(MOO_DEBUG_CFLAGS)         \
    $(CFLAGS)

ggap_LDADD =                    \
    $(GTK_LIBS)                 \
    $(XML_LIBS)                 \
    $(FAM_LIBS)                 \
    $(PYTHON_LDFLAGS)           \
    $(PYTHON_EXTRA_LIBS)

# ggap_LDFLAGS = -export-dynamic -export-symbols-regex "^[^_].*"
ggap_LDFLAGS =

if MOO_OS_MINGW
ggap_LDFLAGS += -mwindows
endif MOO_OS_MINGW

ggap_SOURCES =      \
    ggap-ui.h       \
    gap.c           \
    gap.h           \
    ggap.c          \
    gapapp.c        \
    gapapp.h        \
    gapeditwindow.c \
    gapeditwindow.h \
    gapterm.c       \
    gapterm.h       \
    gaptermwindow.c \
    gaptermwindow.h \
    gapprefs-glade.h

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

EXTRA_DIST =        \
    ggap_res.rc.in  \
    ggap-ui.xml     \
    ggap.opag       \
    glade/gapprefs.glade

BUILT_SOURCES = ggap-ui.h gapprefs-glade.h ggap.c
XML2H = $(srcdir)/moo/mooutils/xml2h.sh
PY2H = $(srcdir)/moo/mooutils/py2h.sh

ggap-ui.h: ggap-ui.xml $(XML2H)
	sh $(XML2H) GGAP_UI $(srcdir)/ggap-ui.xml > ggap-ui.h

gapprefs-glade.h: glade/gapprefs.glade $(XML2H)
	mkdir -p $(mooapp)
	sh $(XML2H) GAP_PREFS_GLADE_UI $(srcdir)/glade/gapprefs.glade > gapprefs-glade.h

ggap.c: ggap.opag
	opag -f _ggap_parse_options -O _ggap_opt_ -A _ggap_arg_ $(srcdir)/ggap.opag $(srcdir)/ggap.c


###########################################################################
# moo
#

moo_prefix = moo
include moo/Makefile.incl
include moo/mooutils/Makefile.incl
include moo/mooedit/Makefile.incl
include moo/mooterm/Makefile.incl
include moo/mooapp/Makefile.incl
include moo/moopython/Makefile.incl

CLEANFILES += $(moo_cleanfiles)

ggap_SOURCES += $(moo_sources)
nodist_ggap_SOURCES += $(moo_nodist_sources)
BUILT_SOURCES += $(moo_built_sources)

if MOO_OS_MINGW
ggap_SOURCES += $(moo_win32_sources)
endif

if MOO_OS_UNIX
ggap_SOURCES += $(moo_unix_sources)
endif

EXTRA_DIST +=                \
    $(moo_sources)           \
    $(moo_win32_sources)     \
    $(moo_unix_sources)      \
    $(moo_built_sources)     \
    $(moo_extra_dist)


###########################################################################
# termhelper
#

if MOO_OS_CYGWIN
noinst_PROGRAMS = termhelper
endif

termhelper_SOURCES =        \
    $(termhelper_sources)   \
    termhelper_res.res

BUILT_SOURCES += termhelper_res.res
nodist_termhelper_SOURCES = termhelper_res.res
CLEANFILES += termhelper_res.res

if MOO_OS_CYGWIN
termhelper_res.res: moo/mooterm/termhelper_res.rc
	$(WINDRES) -i moo/mooterm/termhelper_res.rc --input-format=rc -o termhelper_res.res -O coff
termhelper_LDADD = ggap_res.res
else
termhelper_res.res: moo/mooterm/termhelper_res.rc
	touch termhelper_res.res
endif
