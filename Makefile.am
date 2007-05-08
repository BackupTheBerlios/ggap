AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip foreign nostdinc

SUBDIRS = moo . ggap po

EXTRA_DIST =            \
    makewin             \
    INSTALL             \
    mingw-configure     \
    cygwin-configure    \
    autogen.sh          \
    intltool-extract.in	\
    intltool-merge.in	\
    intltool-update.in	\
    THANKS

@MOO_PO_SUBDIRS_RULE@
BUILT_SOURCES = po-subdirs-stamp

ACLOCAL_AMFLAGS = -I moo/m4 $(ACLOCAL_FLAGS)

ui.xml.example: ggap/ggap-ui.xml
	cp $(srcdir)/ggap/ggap-ui.xml ui.xml.example

ggapdir = $(GGAP_DATA_DIR)
ggap_DATA = ui.xml.example

DISTCLEANFILES =		\
	configargs.h		\
	intltool-extract	\
	intltool-merge		\
	intltool-update		\
	po-subdirs-stamp

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
distuninstallcheck_listfiles = find . -type f -print | grep -v share/mime/ | grep -v share/icons/hicolor

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

all-am: $(generated)

bin-dist: all
	rm -fr _ggap_bin_inst _ggap_bin
	$(MAKE) install DESTDIR=`pwd`/_ggap_bin_inst && \
	mv _ggap_bin_inst/$(prefix) _ggap_bin && \
	cp $(srcdir)/ggap.sh _ggap_bin/ && \
	rm -fr _ggap_bin_inst
