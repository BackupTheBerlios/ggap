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

QT_MODULES=""
MOO_ENABLE_UNIT_TESTS=false
AC_ARG_ENABLE([unit-tests],
  AC_HELP_STRING([--enable-unit-tests],[enable unit tests (default = NO)]),[
  if test "x$enableval" = xyes; then
    MOO_ENABLE_UNIT_TESTS=true
  fi
])
AM_CONDITIONAL(MOO_ENABLE_UNIT_TESTS, $MOO_ENABLE_UNIT_TESTS)
if $MOO_ENABLE_UNIT_TESTS; then
  QT_MODULES="$QT_MODULES testlib"
  MOO_UNIT_TESTS_CFLAGS="-DMOO_ENABLE_UNIT_TESTS"
fi
AC_SUBST(MOO_UNIT_TESTS_CFLAGS)

MOO_AC_QT([$QT_MODULES])

ACLOCAL="${ACLOCAL:-aclocal} $ACLOCAL_FLAGS"

AC_CONFIG_FILES([
Makefile
contrib/Makefile
moows/Makefile
ggap/Makefile
])
AC_OUTPUT
