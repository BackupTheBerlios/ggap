#
# @(#)$Id: configure.ac,v 1.43 2005/06/06 14:53:37 emuntyan Exp $
#

m4_define([ggap_major_version], [0])
m4_define([ggap_minor_version], [5])
m4_define([ggap_micro_version], [92])
m4_define([ggap_version], [ggap_major_version.ggap_minor_version.ggap_micro_version])

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
TERMHELPER_VERSION=$GGAP_VERSION
TERMHELPER_VERSION_MAJOR=$GGAP_VERSION_MAJOR
TERMHELPER_VERSION_MINOR=$GGAP_VERSION_MINOR
TERMHELPER_VERSION_MICRO=$GGAP_VERSION_MICRO
AC_SUBST(TERMHELPER_VERSION)
AC_SUBST(TERMHELPER_VERSION_MAJOR)
AC_SUBST(TERMHELPER_VERSION_MINOR)
AC_SUBST(TERMHELPER_VERSION_MICRO)

AC_PROG_CC
AC_PROG_CC_STDC
AC_HEADER_STDC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_WINDRES
AC_PROG_YACC


################################################################################
#  Various headers and functions needed in various places
#
AC_CHECK_HEADERS(sys/stat.h unistd.h stdlib.h util.h poll.h errno.h io.h fcntl.h sys/types.h sys/wait.h signal.h)
AC_CHECK_FUNCS(mkfifo mktemp poll pipe _pipe kill signal)

# for pcre
AC_CHECK_FUNCS(memmove bcopy)

################################################################################
#  Stuff for terminal emulator, copied from libvte
#
AC_EGREP_CPP(glibc,
		[
		 #include <stdio.h>
		 #ifdef __GLIBC__
		 glibc
		 #endif
		],
		AC_DEFINE(_GNU_SOURCE,1,[Use all glibc features.]))

AC_CHECK_HEADERS(sys/select.h sys/termios.h sys/un.h stropts.h termios.h wchar.h)
AC_HEADER_TIOCGWINSZ

AC_CHECK_FUNCS(cfmakeraw getpgid getpt grantpt unlockpt ptsname ptsname_r recvmsg posix_openpt)

AC_TYPE_SIGNAL
AC_TYPE_SIZE_T
AC_TYPE_PID_T


################################################################################
#  Check debug options
#
AC_CHECK_DEBUG_STUFF([-Wall])


################################################################################
#  Check operating system
#
cygwin_build="no"
mingw_build="no"
unix_build="no"
case $host in
  *-*-mingw32*)
    echo "++++ building for MINGW32 ++++"
    mingw_build="yes"
    ;;
  *-*-cygwin*)
    echo "++++ building for CYGWIN ++++"
    cygwin_build="yes"
    ;;
  *-*-darwin*)
    AC_DEFINE(OS_DARWIN, 1, [darwin])
    unix_build="yes"
    ;;
  *)
    unix_build="yes"
    ;;
esac
AM_CONDITIONAL(UNIX_BUILD, test x$unix_build = "xyes")
AM_CONDITIONAL(MINGW_BUILD, test x$mingw_build = "xyes")
AM_CONDITIONAL(CYGWIN_BUILD, test x$cygwin_build = "xyes")


################################################################################
#  GTK libraries
#
PKG_CHECK_GTK_VERSIONS


################################################################################
#  Python stuff
#
AC_ARG_WITH([pygtk],
  AC_HELP_STRING([--with-pygtk], [whether to compile pygtk support (default = autodetect)]),
  [
    if test x$with_pygtk = "xno"; then
        build_pygtk="no"
    else
        build_pygtk="yes"
    fi
  ], [
    build_pygtk="yes"
])


AC_ARG_WITH([python],
  AC_HELP_STRING([--with-python], [whether to compile python support (default = AUTO)]),
  [
    if test x$with_python = "xno"; then
        build_python="no"
        build_pygtk="no"
    else
        if test x$with_python = "xyes"; then
            python_version=2.2
        else
            python_version=$with_python
        fi
        build_python="yes"
    fi
  ], [
    if test x$build_pygtk = xyes; then
        build_python="yes"
    else
        build_python="auto"
        build_pygtk="no"
    fi
    python_version=2.2
])

if test x$cygwin_build != "xyes" -a x$build_python != "xno"; then
    AC_CHECK_PYTHON($python_version, [
        build_python="yes"
        AC_MSG_NOTICE([compiling python support])
    ], [
        build_python="no"
        build_pygtk="no"
        AC_MSG_WARN([Python not found])
    ])
fi

AM_CONDITIONAL(USE_PYTHON, test x$build_python = "xyes")
if test x$build_python = "xyes"; then
    AC_DEFINE(USE_PYTHON,, [USE_PYTHON])
fi


################################################################################
#  pygtk
#
if test x$cygwin_build != "xyes"; then
    if test x$build_pygtk = xyes; then
        AC_CHECK_PYGTK([$python_version], [
            build_pygtk="yes"
            AC_MSG_NOTICE([compiling pygtk support])
            PYGTK_CODEGEN_DIR=`$PKG_CONFIG --variable=codegendir pygtk-2.0`
            PYGTK_DEFS_DIR=`$PKG_CONFIG --variable=defsdir pygtk-2.0`
            AC_SUBST(PYGTK_CODEGEN_DIR)
            AC_SUBST(PYGTK_DEFS_DIR)
            AC_MSG_NOTICE([pygtk codegen dir: $PYGTK_CODEGEN_DIR])
            AC_MSG_NOTICE([pygtk defs dir: $PYGTK_DEFS_DIR])
        ],[
            build_pygtk="no"
            AC_MSG_NOTICE([usable pygtk not found])
        ])
    fi
fi


AM_CONDITIONAL(USE_PYGTK, test x$build_pygtk = "xyes")
if test x$build_pygtk = "xyes"; then
    AC_DEFINE(USE_PYGTK,, [USE_PYGTK])
fi


################################################################################
#  libxml2
#
AC_ARG_WITH([xml],
    AC_HELP_STRING([--with-xml], [enable xml]),
    [use_xml=$withval],
    [use_xml=auto]
)

if test x$cygwin_build != "xyes"; then
    if test x$use_xml != xno; then
        AC_CHECK_XML_STUFF([found_xml=yes],[found_xml=no])
    fi

    if test x$use_xml = xyes -a x$found_xml = xno; then
        AC_MSG_ERROR([libxml2 library not found])
    fi
    if test x$found_xml = xyes; then
        use_xml=yes
        AC_MSG_NOTICE([compiling with xml support])
        AC_DEFINE(USE_XML,, [use libxml])
    else
        use_xml=no
        AC_MSG_NOTICE([compiling without xml support])
    fi
fi

AM_CONDITIONAL(USE_XML, test x$use_xml = xyes)


################################################################################
#  mime
#
AC_ARG_WITH([mime],
    AC_HELP_STRING([--with-mime], [enable mime]),
    [use_mime=$withval],
    [use_mime=yes]
)

if test x$use_mime != xno; then
    use_mime=yes
    use_xdgmime=yes
    AC_DEFINE(USE_MIME,, [use mime])
    AC_DEFINE(USE_XDGMIME,, [use xdgmime])
fi

AM_CONDITIONAL(USE_MIME, test x$use_mime = xyes)
AM_CONDITIONAL(BUILD_XDGMIME, test x$use_xdgmime = xyes)


################################################################################
#  moo components
#
if test x$cygwin_build = "xyes"; then
    build_mooapp="no"
    build_mooutils="no"
    build_mooui="no"
    build_mooedit="no"
else
    build_mooapp="yes"
    build_mooutils="yes"
    build_mooui="yes"
    build_mooedit="yes"
fi
build_mooterm="yes"

AM_CONDITIONAL(BUILD_MOOAPP,    test "x$build_mooapp"    = "xyes")
AM_CONDITIONAL(BUILD_MOOUTILS,  test "x$build_mooutils"  = "xyes")
AM_CONDITIONAL(BUILD_MOOUI,     test "x$build_mooui"     = "xyes")
AM_CONDITIONAL(BUILD_MOOEDIT,   test "x$build_mooedit"   = "xyes")
AM_CONDITIONAL(BUILD_MOOTERM,   test "x$build_mooterm"   = "xyes")

AM_CONDITIONAL(BUILD_PYMOO, false)

ggap_top_src_dir=`cd $srcdir && pwd`
ggap_top_build_dir=`cd ./$ac_top_builddir && pwd`
MOO_CFLAGS="-I$ggap_top_src_dir/src/moo $GTK_CFLAGS"
MOO_LIBS="$GTK_LIBS"
AC_SUBST(MOO_CFLAGS)
AC_SUBST(MOO_LIBS)
MOO_SRC_PREFIX="src/moo"
AC_SUBST(MOO_SRC_PREFIX)


################################################################################
#  MooEdit stuff
#
MOO_CFLAGS="$MOO_CFLAGS $XML_CFLAGS -I$ggap_top_src_dir/src/moo/mooedit"
MOO_LIBS="$MOO_LIBS $XML_LIBS"
USE_PCRE="yes"

AM_CONDITIONAL(USE_PCRE, test "x$USE_PCRE" = "xyes")

AM_CONDITIONAL(USE_UNRTF, false)


################################################################################
#  Available application profiles
#
AC_ARG_ENABLE([gap],
    AC_HELP_STRING([--enable-gap], [build with GAP support (default=YES)]),
    [enable_gap=$enableval],
    [enable_gap=yes]
)

AC_ARG_ENABLE([sc3],
    AC_HELP_STRING([--enable-sc3], [build with SuperCollider support (default=NO)]),
    [enable_sc3=$enableval],
    [enable_sc3=yes]
)

AC_ARG_ENABLE([generic],
    AC_HELP_STRING([--enable-generic], [build with 'generic application' support (default=NO)]),
    [enable_generic=$enableval],
    [enable_generic=no]
)

# if test "x$enable_gap" = "xyes"; then
#     AC_DEFINE(USE_GAP_PROFILE,1,[Enable GAP application profile])
# fi
# if test "x$enable_sc3" = "xyes"; then
#     AC_DEFINE(USE_SC3_PROFILE,1,[Enable SC3 application profile])
# fi
# if test "x$enable_generic" = "xyes"; then
#     AC_DEFINE(USE_GENERIC_PROFILE,1,[Enable 'Generic' application profile])
# fi
AM_CONDITIONAL(USE_GAP_PROFILE, test "x$enable_gap" = "xyes")
AM_CONDITIONAL(USE_SC3_PROFILE, test "x$enable_sc3" = "xyes")
AM_CONDITIONAL(USE_GENERIC_PROFILE, test "x$enable_generic" = "xyes")


################################################################################
#  Installation directories, etc.
#
if test "x${prefix}" = "xNONE"; then
  ggapprefix=${ac_default_prefix}
else
  ggapprefix=${prefix}
fi

ggapdatadir=share

ggaprootdir=${ggapdatadir}/ggap
NO_PREFIX_GGAP_ROOT_DIR="${ggaprootdir}"
AC_SUBST(NO_PREFIX_GGAP_ROOT_DIR)
GGAP_ROOT_DIR="${ggapprefix}/${ggaprootdir}"
AC_SUBST(GGAP_ROOT_DIR)

AC_DEFINE_UNQUOTED(GGAP_ROOT_DIR, "${ggapprefix}/${ggaprootdir}", [data files go to GGAP_ROOT_DIR])
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${ggapsrcdir}", [top source directory])


MOO_EDIT_LANG_FILES_DIR="${ggapprefix}/${ggaprootdir}/language-specs"
AC_SUBST(MOO_EDIT_LANG_FILES_DIR)
MOO_CFLAGS="$MOO_CFLAGS -DMOO_EDIT_LANG_FILES_DIR=\\\"$MOO_EDIT_LANG_FILES_DIR\\\""


################################################################################
#  Created files
#
AC_OUTPUT([
Makefile
gap/PackageInfo.g
gap/init.g
gap/Makefile
src/Makefile
src/ggap_res.rc
src/gap/Makefile
src/gap/glade/Makefile
src/sc3/Makefile
src/sc3/glade/Makefile
src/generic/Makefile
wininstaller/Makefile
src/moo/Makefile
src/moo/mooapp/Makefile
src/moo/mooapp/glade/Makefile
src/moo/mooedit/Makefile
src/moo/mooedit/gtksourceview/Makefile
src/moo/mooedit/xdgmime/Makefile
src/moo/mooedit/glade/Makefile
src/moo/mooedit/language-specs/Makefile
src/moo/mooterm/Makefile
src/moo/mooterm/glade/Makefile
src/moo/mooui/Makefile
src/moo/mooui/glade/Makefile
src/moo/mooutils/Makefile
src/moo/mooutils/glade/Makefile
src/moo/mooutils/newgtk/Makefile
src/moo/mooutils/pcre/Makefile
src/moo/mooutils/pixmaps/Makefile
src/moo/mooutils/unrtf/Makefile
])


################################################################################
#  Configuration info
#
if test x$cygwin_build != "xyes"; then
    if test "x$mingw_build" = "xyes"; then
        system="MINGW"
    elif test "x$darwin_build" = "xyes"; then
        system="DARWIN"
    else
        system="UNIX"
    fi

    if test x$use_xml != "xyes"; then
        use_xml="no"
    fi

    echo
    echo "    OS                                        = $system"
    echo "    prefix                                    = $ggapprefix"
    echo "    python support                            = $build_python"
    echo "    pygtk support                             = $build_pygtk"
    echo "    xml support                               = $use_xml"
    echo
    echo "    editor lang files go to                   = $MOO_EDIT_LANG_FILES_DIR"
    echo
    echo "    building with GAP support                 = $enable_gap"
    echo "    building with SuperCollider support       = $enable_sc3"
    echo "    building with 'Generic' support           = $enable_generic"
    echo
else
    echo
    echo "    Building termhelper.exe on CYGWIN"
    echo
fi
