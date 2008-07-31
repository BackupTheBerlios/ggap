AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip
ACLOCAL_AMFLAGS = -I m4 $(ACLOCAL_FLAGS)

SUBDIRS = contrib moows ggap

EXTRA_DIST =            \
    autogen.sh          \
    moo-debug.h		\
    moo-macros.h	\
    moo-pimpl.h		\
    moo-unit-tests.h	\
    COPYING		\
    xml2h.sh

# desktopdir = $(datadir)/applications
# desktop_DATA = ggap.desktop
#
# pixmaps_files = ggap.png
# theme_files = ggap.png gap.png
# include ../moo/mk/icon.mk

distuninstallcheck_listfiles = find . -type f -print | grep -v share/mime/ | grep -v share/icons/hicolor

# EXTRA_DIST +=		\
# 	$(desktop_DATA)	\
# 	$(ggap_DATA)	\
# 	ggap.png	\
# 	ggap.ico	\
# 	gap.png

moo-config-dirs.h: moo-config-dirs.h.stamp
moo-config-dirs.h.stamp: Makefile
	{ echo '#define MOO_DATA_DIR "$(MOO_DATA_DIR)"'; \
	  echo '#define MOO_HELP_DIR "$(MOO_HELP_DIR)"'; \
	} | sed '/""/d' > moo-config-dirs.h.tmp || exit 1
	cmp -s moo-config-dirs.h moo-config-dirs.h.tmp || \
	    mv moo-config-dirs.h.tmp moo-config-dirs.h || exit 1
	rm -f moo-config-dirs.h.tmp
	echo stamp > moo-config-dirs.h.stamp
BUILT_SOURCES = moo-config-dirs.h.stamp
CLEANFILES = moo-config-dirs.h.stamp moo-config-dirs.h

include mac/Makefile.incl
include win32/Makefile.incl
