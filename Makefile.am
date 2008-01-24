AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip foreign nostdinc

moo =
if USE_MOO_SUBDIR
moo += moo
endif USE_MOO_SUBDIR

DIST_SUBDIRS = $(moo) ggap po
SUBDIRS = $(moo) .

if !MOO_OS_CYGWIN
SUBDIRS += ggap
endif

# if MOO_ENABLE_NLS
# SUBDIRS += po
# endif

# if GGAP_BUILD_DOCS
# if !MOO_OS_CYGWIN
# SUBDIRS += help
# endif
# endif

EXTRA_DIST =            \
    INSTALL             \
    autogen.sh          \
    intltool-extract.in	\
    intltool-merge.in	\
    intltool-update.in	\
    THANKS		\
    scripts/ggap-full.sh\
    scripts/ggap.sh

@MOO_PO_SUBDIRS_RULE@
BUILT_SOURCES = po-subdirs-stamp

ACLOCAL_AMFLAGS = -I moo/m4 $(ACLOCAL_FLAGS)

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
	gap.png

generated =			\
	ggap/gap/PackageInfo.g	\
	ggap/gap/init.g		\
	wininstaller/ggap.iss

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
