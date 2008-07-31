m4_define([ggap_version], [1.9.3])
m4_define([ggap_api_version], [1.3])

AC_INIT([GGAP],[ggap_version],[muntyan@tamu.edu],[ggap])
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_HEADERS(config.h)

MOO_DEFINE_VERSIONS(ggap,[ggap_version])

GGAP_API_VERSION=\"ggap_api_version\"
AC_SUBST(GGAP_API_VERSION)
AC_DEFINE_UNQUOTED(GGAP_API_VERSION, [$GGAP_API_VERSION], [GGAP_API_VERSION])

MOO_PACKAGE_NAME=ggap
AC_DEFINE_UNQUOTED([MOO_PACKAGE_NAME], "$MOO_PACKAGE_NAME", [package name])
MOO_DATA_DIR="${datadir}/$MOO_PACKAGE_NAME"
AC_SUBST(MOO_DATA_DIR)
MOO_HELP_DIR="${htmldir}/help"
AC_SUBST(MOO_HELP_DIR)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

MOO_AC_CHECK_OS
if test x$MOO_OS_MINGW = "xyes"; then
  AC_ARG_VAR([WINDRES], [windres])
  AC_CHECK_TOOL(WINDRES, windres, :)
fi

MOO_AC_PCH
MOO_AC_QT

ACLOCAL="${ACLOCAL:-aclocal} $ACLOCAL_FLAGS"

# AC_CHECK_HEADERS([readline/readline.h],[READLINE_LIBS='-lreadline'],[READLINE_LIBS=])
# AC_SUBST(READLINE_LIBS)

# zlib_missing=true
# AC_CHECK_HEADER([zlib.h],[
#   AC_CHECK_LIB([z],[gzdopen],[
#     ZLIB="-lz"
#     zlib_missing=false
# ])])
# if $zlib_missing; then
#   AC_MSG_ERROR([libz missing])
# fi

# GGAP_API_VERSION=\"1.0\"
# AC_SUBST(GGAP_API_VERSION)
# AC_DEFINE_UNQUOTED(GGAP_API_VERSION, [$GGAP_API_VERSION], [GGAP_API_VERSION])


AC_CONFIG_FILES([
Makefile
contrib/Makefile
moows/Makefile
ggap/Makefile
])
AC_OUTPUT
