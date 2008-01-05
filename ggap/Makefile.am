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
    $(regex_cflags)			\
    $(MOO_CFLAGS)			\
    $(MOO_DEBUG_CFLAGS)			\
    $(GTK_CFLAGS)			\
    -DGGAP_LOCALE_DIR=\"$(datadir)/locale\"	\
    -DDISABLE_OLD_STUFF
AM_OBJCFLAGS = $(AM_CFLAGS) $(MOO_OBJCFLAGS)

testparser_LDADD = $(MOO_LIBS) $(MOO_OBJC_LIBS) $(READLINE_LIBS)

ggap_LDADD = moows/libmoows.la ../moo/moo/libmoo.la $(MOO_OBJC_LIBS) $(ZLIB)
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

ws_sources =				\
	gapworksheet.c			\
	gapworksheet.h			\
	gapwscompletion.c		\
	gapwscompletion.h		\
	gapwsview.c			\
	gapwsview.h			\
	gapwswindow.c			\
	gapwswindow.h

editor_sources =			\
	gapeditwindow.c			\
	gapeditwindow.h

ggap_SOURCES =				\
	$(ws_sources)			\
	ggap-ui.h			\
	gapwswindow-ui.h		\
	gap.c				\
	gap.h				\
	ggap-i18n.h			\
	ggap-i18n.c			\
	ggap.c				\
	ggapfile.h			\
	ggapfile.c			\
	gapapp.c			\
	gapapp.h			\
	gapparser.c			\
	gapparser.h			\
	gapparser-priv.h		\
	gapparser-yacc.c		\
	gapparser-yacc.h		\
	gapoutput.c			\
	gapoutput.h			\
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
    $(editor_sources)	\
    ggap_res.rc.in	\
    pipehelper_res.rc.in\
    ggap-ui.xml		\
    gapwswindow-ui.xml	\
    gapparser-yacc.y	\
    glade/gapprefs.glade\
    glade/usermenu.glade\
    ggap.xml.in

BUILT_SOURCES =		\
	gapparser-yacc.c\
	gapparser-yacc.h\
	ggap-ui.h	\
	gapwswindow-ui.h\
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
gapwswindow-ui.h: gapwswindow-ui.xml $(XML2H)
	sh $(XML2H) GAP_WS_WINDOW_UI $(srcdir)/gapwswindow-ui.xml > gapwswindow-ui.h.tmp && \
	mv gapwswindow-ui.h.tmp gapwswindow-ui.h

gapprefs-glade.h: glade/gapprefs.glade $(XML2H)
	sh $(XML2H) GAP_PREFS_GLADE_UI $(srcdir)/glade/gapprefs.glade > gapprefs-glade.h


packagesdir = $(mimedir)/packages
packages_DATA = ggap.xml
CLEANFILES += ggap.xml
update_mime = update-mime-database $(DESTDIR)${mimedir}
@MOO_INTLTOOL_XML_RULE@

if !MOO_OS_MINGW
install-data-hook:
	if echo "Updating mime database... " && $(update_mime); then		\
		echo "Done.";							\
	else									\
		echo "*** ";							\
		echo "*** Mime database not updated. After install, run this:";	\
		echo $(update_mime);						\
		echo "*** ";							\
	fi
uninstall-hook:
	echo "Updating mime database" && $(update_mime) && echo "Done."
else MOO_OS_MINGW
install-data-hook:
	cp /usr/share/mime/packages/freedesktop.org.xml $(DESTDIR)${mimedir}/packages/ && \
	$(update_mime)
endif
