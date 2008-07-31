AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip

DIST_SUBDIRS = moo moo-newdoc/mooui ggap
SUBDIRS = moo .
# SUBDIRS = .

if !MOO_OS_CYGWIN
SUBDIRS += moo-newdoc/mooui ggap
endif

# if GGAP_BUILD_DOCS
# if !MOO_OS_CYGWIN
# SUBDIRS += help
# endif
# endif

EXTRA_DIST =            \
    INSTALL             \
    autogen.sh          \
    THANKS		\
    scripts/ggap-full.sh\
    scripts/ggap.sh

ACLOCAL_AMFLAGS = -I moo/m4 $(ACLOCAL_FLAGS)

DISTCLEANFILES = configargs.h

desktopdir = $(datadir)/applications
desktop_DATA = ggap.desktop

pixmaps_files = ggap.png
theme_files = ggap.png gap.png
include ../moo/mk/icon.mk

distuninstallcheck_listfiles = find . -type f -print | grep -v share/mime/ | grep -v share/icons/hicolor

EXTRA_DIST +=		\
	$(desktop_DATA)	\
	$(ggap_DATA)	\
	ggap.png	\
	ggap.ico	\
	gap.png

generated = wininstaller/ggap.iss

all-am: $(generated)

_bin-dist: all
	rm -fr ggap-bin-inst ggap-bin
	$(MAKE) install DESTDIR=`pwd`/ggap-bin-inst && \
	mv ggap-bin-inst/$(prefix) ggap-bin && \
	cp $(srcdir)/scripts/$(ggap_script) ggap-bin/ggap.sh && \
	rm -fr ggap-bin-inst
bin-dist-full:
	$(MAKE) _bin-dist ggap_script=ggap-full.sh
bin-dist: all
	$(MAKE) _bin-dist ggap_script=ggap.sh
