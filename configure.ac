#
# ggap/configure.ac
#

m4_define([ggap_version], [0.9.0])

AC_INIT([GGAP],[ggap_version],[muntyan@tamu.edu],[ggap])
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_HEADERS(config.h)

MOO_DEFINE_VERSIONS(ggap,[ggap_version])

export MOO_PACKAGE_NAME=ggap

AC_PROG_CC
AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"

AC_CHECK_HEADERS([readline/readline.h],[READLINE_LIBS='-lreadline'],[READLINE_LIBS=])
AC_SUBST(READLINE_LIBS)

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
MOO_AC_SRCDIR(moo/moo)
MOO_AC_I18N(ggap,no)
MOO_AC_FLAGS
MOO_AM_MK

GGAP_API_VERSION=\"1.0\"
AC_SUBST(GGAP_API_VERSION)
AC_DEFINE_UNQUOTED(GGAP_API_VERSION, [$GGAP_API_VERSION], [GGAP_API_VERSION])

export MOO_BUILD_MEDIT=false
AC_CONFIG_SUBDIRS(moo)


AC_CONFIG_FILES([
Makefile
config.py
moo-newdoc/mooui/Makefile
ggap/Makefile
ggap/ggap_res.rc
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
