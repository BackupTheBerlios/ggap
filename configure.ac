#
# ggap/configure.ac
#

m4_define([ggap_version], [0.9.0])

AC_INIT([GGAP],[ggap_version],[muntyan@tamu.edu],[ggap])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(config.h)

MOO_DEFINE_VERSIONS(ggap,[ggap_version])

export MOO_PACKAGE_NAME=ggap

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

# remove this
MOO_AC_FUNCS

GETTEXT_PACKAGE=ggap
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Name of the gettext package.])

zlib_missing=true
AC_CHECK_HEADER([zlib.h],[
  AC_CHECK_LIB([z],[gzdopen],[
    ZLIB="-lz"
    zlib_missing=false
])])
if $zlib_missing; then
  AC_MSG_ERROR([libz missing])
fi

MOO_AC_CHECK_OS
_MOO_AC_CONFIGARGS_H
MOO_AC_DEBUG
MOO_AC_TOOLS(moo/moo)

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
mimedir="${datadir}/mime"
AC_SUBST(mimedir)

# AC_ARG_ENABLE([help],
#   AC_HELP_STRING(--disable-help, [Disable building html help files (default = auto).]),
#   [:],[enable_help=auto])

# if test "x$enable_help" = xauto -o "x$enable_help" = xyes; then
#   _MOO_AC_CHECK_XSLT_DOCBOOK([_moo_xslt_working=yes],[_moo_xslt_working=no])
#   if test $_moo_xslt_working = yes; then
#     enable_help=yes
#   elif test "x$enable_help" = xauto; then
#     enable_help=no
#   else
#     AC_MSG_ERROR([xsltproc will not work])
#   fi
# fi
enable_help=no
if test "x$enable_help" = xyes; then
  export GGAP_BUILD_DOCS=yes
  export MOO_ENABLE_HELP=yes
  AC_DEFINE(MOO_ENABLE_HELP, [1], [enable help functionality])
else
  export GGAP_BUILD_DOCS=no
  export MOO_ENABLE_HELP=no
fi

AM_CONDITIONAL(GGAP_BUILD_DOCS, test "x$GGAP_BUILD_DOCS" = xyes)


m4_if([yes],[no],[
  PKG_CHECK_MODULES(MOO, moo)
  USE_MOO_SUBDIR=false
],[
  export MOO_USE_PIPE_INPUT=yes
  export MOO_BUILD_MEDIT=false
  export MOO_BUILD_DOCS=no
  AC_CONFIG_SUBDIRS(moo)
  MOO_CFLAGS='-I$(top_builddir)/moo/moo -I$(top_srcdir)/moo/moo'
  MOO_LIBS='$(top_builddir)/moo/moo/libmoo.la'
  USE_MOO_SUBDIR=true
  AC_DEFINE(USE_MOO_SUBDIR,[1],[USE_MOO_SUBDIR])
])
AC_SUBST(MOO_CFLAGS)
AC_SUBST(MOO_LIBS)
AM_CONDITIONAL(USE_MOO_SUBDIR,[$USE_MOO_SUBDIR])


AC_CONFIG_FILES([
Makefile
config.py
moo-newdoc/mooui/Makefile
ggap/Makefile
ggap/ggap_res.rc
ggap/gap/PackageInfo.g
ggap/gap/init.g
ggap/gap/Makefile
ggap/moows/Makefile
wininstaller/ggap.iss
])
AC_OUTPUT


################################################################################
#  Configuration info
#
echo
echo "    OS........................... $MOO_OS_NAME"
if test "x$MOO_OS_CYGWIN" != "xyes"; then
echo "    installation prefix.......... $prefix"
echo "    shared package goes to....... $GGAP_GAP_DIR"
echo
fi
