SUBDIRS = gap moows .

EXTRA_PROGRAMS = ggap
bin_PROGRAMS =
noinst_PROGRAMS = testparser

if !MOO_OS_CYGWIN
bin_PROGRAMS += ggap
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
	$(editor_sources)		\
	gapwswindow-ui.h		\
	gaptermwindow-ui.h		\
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
	gapterm.c			\
	gapterm.h			\
	gaptermwindow.c			\
	gaptermwindow.h			\
	ggap-credits.h

gapparser-yacc.h: gapparser-yacc.c
	touch $(srcdir)/gapparser-yacc.h
gapparser-yacc.c: gapparser-yacc.y
	bison -v -o $(srcdir)/gapparser-yacc.c -d $(srcdir)/gapparser-yacc.y

nodist_ggap_SOURCES = ggap_res.res
CLEANFILES = ggap_res.res

if MOO_OS_MINGW
ggap_res.res: ggap_res.rc
	$(WINDRES) -i ggap_res.rc --input-format=rc -o ggap_res.res -O coff
ggap_SOURCES += ggap_res.res
ggap_LDADD += ggap_res.res
else ! MOO_OS_MINGW
ggap_res.res:
	echo dummy > ggap_res.res
endif ! MOO_OS_MINGW

EXTRA_DIST =			\
    $(editor_sources)		\
    ggap_res.rc.in		\
    gaptermwindow-ui.xml	\
    gapwswindow-ui.xml		\
    gapparser-yacc.y		\
    glade/usermenu.glade	\
    ggap.xml.in

BUILT_SOURCES =			\
	gapparser-yacc.c	\
	gapparser-yacc.h	\
	gaptermwindow-ui.h	\
	gapwswindow-ui.h	\
	ggap.c			\
	ggap-credits.h

XML2H = $(top_srcdir)/moo/moo/mooutils/xml2h.sh
PY2H = $(top_srcdir)/moo/moo/mooutils/py2h.sh

ggap-credits.h: ../THANKS $(XML2H)
	$(SHELL) $(XML2H) THANKS $(srcdir)/../THANKS > ggap-credits.h.tmp && \
	mv ggap-credits.h.tmp ggap-credits.h
gapwswindow-ui.h: gapwswindow-ui.xml $(XML2H)
	sh $(XML2H) GAP_WS_WINDOW_UI $(srcdir)/gapwswindow-ui.xml > gapwswindow-ui.h.tmp && \
	mv gapwswindow-ui.h.tmp gapwswindow-ui.h
gaptermwindow-ui.h: gaptermwindow-ui.xml $(XML2H)
	sh $(XML2H) GAP_TERM_WINDOW_UI $(srcdir)/gaptermwindow-ui.xml > gaptermwindow-ui.h.tmp && \
	mv gaptermwindow-ui.h.tmp gaptermwindow-ui.h

%-glade.h: glade/%.glade $(XML2H)
	$(SHELL) $(XML2H) `basename "$*" | sed -e "s/-/_/"`_glade_xml $< > $@.tmp && mv $@.tmp $@
glade_files =				\
	glade/gapprefs.glade
glade_sources = $(patsubst glade/%.glade,%-glade.h,$(glade_files))
ggap_SOURCES += $(glade_sources)
BUILT_SOURCES += $(glade_sources)
CLEANFILES += $(glade_sources)
EXTRA_DIST += $(glade_files)

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
