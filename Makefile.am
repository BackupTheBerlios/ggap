AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip foreign nostdinc

SUBDIRS = . ggap

m4files =			\
	m4/moo-comps.m4		\
	m4/moo-debug.m4		\
	m4/moo-dirs.m4		\
	m4/moo-fam.m4		\
	m4/moo-flags.m4		\
	m4/moo-funcs.m4		\
	m4/moo-gtk.m4		\
	m4/moo-i18n.m4		\
	m4/moo-os.m4		\
	m4/moo-pcre.m4		\
	m4/moo-progs.m4		\
	m4/moo-pygtk.m4		\
	m4/moo-python.m4	\
	m4/moo-version.m4	\
	m4/moo-xdgmime.m4	\
	m4/moo-xml.m4

EXTRA_DIST =            \
    $(m4files)          \
    makewin             \
    INSTALL             \
    mingw-configure     \
    cygwin-configure    \
    autogen.sh          \
    intltool-extract.in	\
    intltool-merge.in	\
    intltool-update.in	\
    THANKS

ACLOCAL_AMFLAGS = -I m4 $(ACLOCAL_FLAGS)

ui.xml.example: ggap/ggap-ui.xml
	cp $(srcdir)/ggap/ggap-ui.xml ui.xml.example

ggapdir = $(GGAP_DATA_DIR)
ggap_DATA = ui.xml.example

DISTCLEANFILES =		\
	m4-stamp		\
	configargs.h		\
	intltool-extract	\
	intltool-merge		\
	intltool-update

desktopdir = $(datadir)/applications
desktop_DATA = ggap.desktop
appicondir = $(datadir)/pixmaps
appicon_DATA = ggap.png
hicolor = $(datadir)/icons/hicolor
iconthemedir = $(hicolor)/48x48/apps
icontheme_DATA = ggap.png gap.png

update_icon_cache = gtk-update-icon-cache -f -t $(DESTDIR)$(hicolor)
install-data-hook:
	if echo "Updating icon cache" && $(update_icon_cache); then		\
		echo "Done.";							\
	else									\
		echo "*** GTK icon cache not updated. After install, run this:";\
		echo $(update_icon_cache);					\
	fi
uninstall-hook:
	echo "Updating icon cache" && $(update_icon_cache) && echo "Done."
distuninstallcheck_listfiles = find . -type f -print | grep -v share/icons/hicolor

EXTRA_DIST +=		\
	$(desktop_DATA)	\
	$(ggap_DATA)	\
	ggap.png	\
	ggap.ico	\
	gap.png		\
	xml2h.sh	\
	py2h.sh

generated =			\
	ggap/gap/PackageInfo.g	\
	ggap/gap/init.g		\
	wininstaller/ggap.iss

all-am: $(generated) m4-stamp

m4-stamp: $(m4files)
	echo stamp > m4-stamp
	for f in $(m4files); do						\
		if test $(srcdir)/$$f -nt $(srcdir)/configure; then	\
			touch $(srcdir)/configure.ac;			\
		fi							\
	done
