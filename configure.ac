#
# ggap/configure.ac
#

m4_define([ggap_major_version], [0])
m4_define([ggap_minor_version], [6])
m4_define([ggap_micro_version], [35])
m4_define([ggap_version], [ggap_major_version.ggap_minor_version.ggap_micro_version])

m4_define([moo_major_version], [0])
m4_define([moo_minor_version], [6])
m4_define([moo_micro_version], [7])
m4_define([moo_version], [moo_major_version.moo_minor_version.moo_micro_version])

AC_INIT(GGAP, [ggap_version], [muntyan@math.tamu.edu], ggap)
AM_INIT_AUTOMAKE(ggap, [ggap_version])

AC_CONFIG_HEADERS(config.h)

AC_DEFINE(GGAP_VERSION, ["ggap_version"], "GGAP package version")

GGAP_VERSION=\"ggap_version\"
GGAP_VERSION_MAJOR=ggap_major_version
GGAP_VERSION_MINOR=ggap_minor_version
GGAP_VERSION_MICRO=ggap_micro_version
AC_SUBST(GGAP_VERSION)
AC_SUBST(GGAP_VERSION_MAJOR)
AC_SUBST(GGAP_VERSION_MINOR)
AC_SUBST(GGAP_VERSION_MICRO)
MOO_DEFINE_VERSIONS

AC_PROG_CC
AC_PROG_CC_STDC
AC_HEADER_STDC
# AC_LIBTOOL_WIN32_DLL
# AC_PROG_LIBTOOL
AC_PROG_INSTALL
MOO_AC_PROG_WINDRES

#  Check debug options
MOO_AC_DEBUG
#  Check operating system
MOO_AC_CHECK_OS
#  GTK libraries
MOO_PKG_CHECK_GTK_VERSIONS
#  File monitoring
MOO_AC_FAM
#  libxml2
MOO_AC_XML
#  xdgmime
MOO_AC_XDGMIME
#  Python stuff
MOO_AC_PYGTK
#  Different stuff needed for moo
MOO_AC_FUNCS
# pcre
MOO_AC_PCRE


AM_CONDITIONAL(MOO_BUILD_UTILS, test x$MOO_OS_CYGWIN != xyes)
AM_CONDITIONAL(MOO_BUILD_EDIT, test x$MOO_OS_CYGWIN != xyes)
AM_CONDITIONAL(MOO_BUILD_APP, test x$MOO_OS_CYGWIN != xyes)
AM_CONDITIONAL(MOO_BUILD_TERM,  true)

if test x$MOO_OS_CYGWIN != xyes; then
    MOO_BUILD_UTILS=1
    MOO_BUILD_EDIT=1
    MOO_BUILD_APP=1
    AC_DEFINE(MOO_BUILD_UTILS, 1, [build mooutils])
    AC_DEFINE(MOO_BUILD_EDIT, 1, [build mooedit])
    AC_DEFINE(MOO_BUILD_APP, 1, [build mooapp])
else
    MOO_BUILD_UTILS=0
    MOO_BUILD_EDIT=0
    MOO_BUILD_APP=0
fi

AC_DEFINE(MOO_BUILD_TERM, 1, [build mooterm])
MOO_BUILD_TERM=1

AC_SUBST(MOO_BUILD_UTILS)
AC_SUBST(MOO_BUILD_EDIT)
AC_SUBST(MOO_BUILD_TERM)
AC_SUBST(MOO_BUILD_APP)

AM_CONDITIONAL(MOO_BUILD_PLUGINS,   false)
AM_CONDITIONAL(MOO_STANDALONE,      false)


################################################################################
#  Installation directories, etc.
#
MOO_SRC_PREFIX="ggap/moo"
AC_SUBST(MOO_SRC_PREFIX)
MOO_AC_RELOC
MOO_AC_SET_DIRS(ggap)
MOO_AC_FLAGS

GGAP_DATA_DIR="${MOO_DATA_DIR}"
AC_SUBST(GGAP_DATA_DIR)
MOO_CFLAGS="$MOO_CFLAGS -DGGAP_DATA_DIR=\\\"${GGAP_DATA_DIR}\\\""

GGAP_GAP_DIR="${GGAP_DATA_DIR}/pkg/ggap"
AC_SUBST(GGAP_GAP_DIR)
MOO_CFLAGS="$MOO_CFLAGS -DGGAP_GAP_DIR=\\\"${GGAP_GAP_DIR}\\\""


################################################################################
#  Created files
#
AC_OUTPUT([
Makefile
m4/Makefile
pkg/PackageInfo.g
pkg/init.g
pkg/Makefile
ggap/Makefile
ggap/ggap_res.rc
ggap/moo/mooterm/termhelper_res.rc
ggap/moo/moo.pc
ggap/moo/moo-version.h
])


################################################################################
#  Configuration info
#
if test x$MOO_OS_CYGWIN != "xyes"; then
    if test "x$MOO_OS_MINGW" = "xyes"; then
        system="MINGW"
    elif test "x$MOO_OS_DARWIN" = "xyes"; then
        system="DARWIN"
    else
        system="UNIX"
    fi

    echo
    echo "    OS                        = $system"
    echo "    prefix                    = $prefix"
    echo "    python support            = $MOO_USE_PYGTK"
    echo
    echo "    editor lang files go to   = $MOO_TEXT_LANG_FILES_DIR"
    echo "    shared package goes to    = $GGAP_GAP_DIR"
    echo
else
    echo
    echo "    Building termhelper.exe on CYGWIN"
    echo
fi
