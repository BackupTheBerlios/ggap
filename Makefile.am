AUTOMAKE_OPTIONS = dist-bzip2 foreign nostdinc

DIST_SUBDIRS = pkg ggap m4
SUBDIRS = pkg ggap

EXTRA_DIST =            \
    makewin             \
    INSTALL             \
    mingw-configure     \
    cygwin-configure    \
    autogen.sh

ACLOCAL_AMFLAGS = -I m4 $(ACLOCAL_FLAGS)

ui.xml.example: ggap/ggap-ui.xml
	cp $(srcdir)/ggap/ggap-ui.xml ui.xml.example

ggapdir = $(MOO_DATA_DIR)
ggap_DATA = ui.xml.example

desktopdir = $(datadir)/applications
desktop_DATA = ggap.desktop
mimedir = $(datadir)/mime
mime_packagesdir = $(mimedir)/packages
mime_packages_DATA = gap.xml
appicondir = $(datadir)/pixmaps
appicon_DATA = ggap/moo/mooutils/pixmaps/ggap.png
iconthemedir = $(datadir)/icons/hicolor/48x48/apps
icontheme_DATA = ggap/moo/mooutils/pixmaps/ggap.png


update_mime = update-mime-database $(mimedir)

install-data-hook:
	if echo "Updating mime database." && $(update_mime); then		\
		echo "Done.";							\
	else									\
		echo "*** Mime database not updated. After install, run this:";	\
		echo $(update_mime);						\
	fi


EXTRA_DIST += $(desktop_DATA) $(ggap_DATA) $(mime_packages_DATA)

generated = 			\
	pkg/ggap/PackageInfo.g	\
	pkg/ggap/init.g		\
	wininstaller/ggap.iss

all-am: $(generated)
