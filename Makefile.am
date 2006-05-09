AUTOMAKE_OPTIONS = dist-bzip2 foreign nostdinc

DIST_SUBDIRS = pkg ggap m4
SUBDIRS = pkg ggap

EXTRA_DIST =            \
    makewin             \
    INSTALL             \
    mingw-configure     \
    cygwin-configure    \
    autogen.sh          \
    ui.xml.example

ACLOCAL_AMFLAGS = -I m4 $(ACLOCAL_FLAGS)

ui.xml.example: ggap/ggap-ui.xml
	cp $(srcdir)/ggap/ggap-ui.xml ui.xml.example

ggapdir = $(MOO_DATA_DIR)
ggap_DATA = ui.xml.example

desktopdir = $(datadir)/applications
desktop_DATA = ggap.desktop
appicondir = $(datadir)/pixmaps
appicon_DATA = ggap/moo/mooutils/pixmaps/ggap.png
iconthemedir = $(datadir)/icons/hicolor/48x48/apps
icontheme_DATA = ggap/moo/mooutils/pixmaps/ggap.png
EXTRA_DIST += $(desktop_DATA)
