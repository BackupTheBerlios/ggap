#
# ggap/configure.ac
#

m4_define([ggap_version], [0.8.0])

AC_INIT([GGAP],[ggap_version],[muntyan@tamu.edu],[ggap])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(config.h)

MOO_DEFINE_VERSIONS(ggap,[ggap_version])

export MOO_BUILD_MEDIT=false
export MOO_PACKAGE_NAME=ggap
export MOO_USE_PIPE_INPUT=true

AC_PROG_CC
AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
MOO_AC_PROGS

MOO_AC_I18N(ggap)

ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"

PKG_CHECK_MODULES(GTK, gtk+-2.0)
AC_CHECK_HEADERS([readline/readline.h],[READLINE_LIBS='-lreadline'],[READLINE_LIBS=])
AC_SUBST(READLINE_LIBS)

MOO_AC_CHECK_OS
_MOO_AC_CONFIGARGS_H
MOO_AC_DEBUG

MOO_PKG_CHECK_GTK_VERSIONS

MOO_INI_IN_IN_RULE='%.ini.desktop.in: %.ini.desktop.in.in $(top_builddir)/config.status ; cd $(top_builddir) && $(SHELL) ./config.status --file=$(subdir)/[$]@'
MOO_INI_IN_RULE='%.ini: %.ini.in $(top_builddir)/config.status ; cd $(top_builddir) && $(SHELL) ./config.status --file=$(subdir)/[$]@'
MOO_WIN32_RC_RULE='%.res: %.rc.in $(top_builddir)/config.status ; cd $(top_builddir) && $(SHELL) ./config.status --file=$(subdir)/[$]*.rc && cd $(subdir) && $(WINDRES) -i [$]*.rc --input-format=rc -o [$]@ -O coff && rm [$]*.rc'
AC_SUBST(MOO_INI_IN_IN_RULE)
AC_SUBST(MOO_INI_IN_RULE)
AC_SUBST(MOO_WIN32_RC_RULE)

GGAP_DATA_DIR="${datadir}/ggap"
AC_SUBST(GGAP_DATA_DIR)
GGAP_GAP_DIR="${GGAP_DATA_DIR}/pkg"
AC_SUBST(GGAP_GAP_DIR)

export MOO_USE_PIPE_INPUT=yes
AC_CONFIG_SUBDIRS(moo)

AC_CONFIG_FILES([
Makefile
config.py
ggap/Makefile
ggap/ggap_res.rc
ggap/pipehelper_res.rc
ggap/gap/PackageInfo.g
ggap/gap/init.g
ggap/gap/Makefile
po/Makefile.in
wininstaller/ggap.iss
])
AC_OUTPUT


################################################################################
#  Configuration info
#
echo
echo "    OS........................... $MOO_OS_NAME"
echo "    installation prefix.......... $prefix"
echo "    shared package goes to....... $GGAP_GAP_DIR"
echo
