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

desktopdir = $(datadir)/applications
desktop_DATA = ggap.desktop
appicondir = $(datadir)/pixmaps
appicon_DATA = ggap/moo/mooutils/pixmaps/ggap.png
EXTRA_DIST += $(desktop_DATA)
