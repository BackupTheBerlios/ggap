# MOO_AC_QT([additional-modules])
AC_DEFUN([MOO_AC_QT],[
AC_REQUIRE([MOO_AC_CHECK_OS])

AC_ARG_VAR([QT_MOC], [path to moc utility])
AC_ARG_VAR([QT_UIC], [path to uic utility])
AC_ARG_VAR([QT_RCC], [path to rcc utility])
AC_CHECK_TOOL(QT_MOC, moc, [AC_MSG_ERROR([moc not found])])
AC_CHECK_TOOL(QT_UIC, uic, [AC_MSG_ERROR([uic not found])])
AC_CHECK_TOOL(QT_RCC, rcc, [AC_MSG_ERROR([rcc not found])])
AC_ARG_VAR([QT_CXXFLAGS], [Qt compilation flags])
AC_ARG_VAR([QT_LIBS], [Qt linker flags])

if test "x$cross_compiling" = xyes; then
  _MOO_AC_QT_CROSS([$1])
else
  _MOO_AC_QT_NATIVE([$1])
fi

_MOO_AC_QT_CHECK
])


AC_DEFUN([_MOO_AC_QT_CHECK],[
  AC_MSG_CHECKING([whether compilation with Qt works])

  save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS $QT_CXXFLAGS"
  save_LIBS="$LIBS"
  LIBS="$LIBS $QT_LIBS"

  AC_LANG_PUSH(C++)
  AC_LINK_IFELSE([AC_LANG_SOURCE([[
  #include <QtGui>

  class Foobar : public QTextEdit {
  };

  int main (int argc, char *argv[])
  {
    QApplication app(argc, argv);
    Foobar foo;
    foo.show();
    return app.exec();
  }
  ]])],[
    AC_MSG_RESULT([yes])
  ],[
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Can't compile Qt programs])
  ])

  AC_LANG_POP

  CXXFLAGS="$save_CXXFLAGS"
  LIBS="$save_LIBS"
])


AC_DEFUN([_MOO_AC_QT_NATIVE],[

AC_ARG_VAR([QMAKE], [path to qmake utility])
AC_CHECK_TOOL(QMAKE, qmake, [AC_MSG_ERROR([qmake not found])])

_moo_ac_success=true
_moo_ac_temp_dir=moo-ac-qt-test-dir

# if flags are set, do nothing
if test "x$QT_CXXFLAGS" = x -a "x$QT_LIBS" = x; then
  AC_MSG_CHECKING(for Qt)

  mkdir -p $_moo_ac_temp_dir || _moo_ac_success=false

  if $_moo_ac_success; then
    cd $_moo_ac_temp_dir
    echo > conftest.h
    echo > conftest.cpp

    cat > conftest.pro <<MOOEOFEOF
CONFIG += qt thread
QT += xml script $1
TEMPLATE = app
TARGET = conftest
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
HEADERS += conftest.h
SOURCES += conftest.cpp
DEFINES += QT_NO_CAST_TO_ASCII
MOOEOFEOF

    cat > Makefile <<\MOOEOFEOF
include Makefile.incl
moo_ac_print_flags:
	echo $(DEFINES) $(INCPATH) > CXXFLAGS || exit 1
	echo $(LIBS) > LIBS || exit 1
MOOEOFEOF

    _moo_ac_qmake_spec=
    if test "x$MOO_OS_DARWIN" = "xyes"; then
      _moo_ac_qmake_spec="-spec macx-g++"
    fi
    "$QMAKE" -makefile $_moo_ac_qmake_spec -o Makefile.incl || _moo_ac_success=false

    if $_moo_ac_success; then
      make moo_ac_print_flags || _moo_ac_success=false
    fi

    if $_moo_ac_success; then
      QT_CXXFLAGS=`cat CXXFLAGS`
      QT_LIBS=`cat LIBS`
    fi

    cd ..
  fi

  if $_moo_ac_success; then
    AC_MSG_RESULT(found)
    echo Qt compiler flags: $QT_CXXFLAGS
    echo Qt linker flags: $QT_LIBS
  else
    AC_MSG_ERROR(failed)
  fi
fi

rm -fr $_moo_ac_temp_dir

]) # end of MOO_AC_QT


AC_DEFUN([_MOO_AC_QT_CROSS],[
  if test x$MOO_OS_MINGW = xyes; then
    _MOO_AC_QT_CROSS_MINGW([$1])
  else
    _MOO_AC_QT_NATIVE([$1])
  fi
])

AC_DEFUN([_MOO_AC_QT_CROSS_MINGW],[
if test "x$QT_CXXFLAGS" = x -a "x$QT_LIBS" = x; then
  AC_MSG_CHECKING(for Qt)

  AC_ARG_VAR([QT_PREFIX], [Qt prefix])

  if test "x$QT_PREFIX" = x; then
    AC_MSG_ERROR([QT_PREFIX is not set])
  fi

  QT_CXXFLAGS="-DQT_SHARED -DQT_NO_CAST_TO_ASCII \
-I$QT_PREFIX/include -I$QT_PREFIX/mkspecs/win32-g++"
  QT_LIBS="-L$QT_PREFIX/lib"

  for _moo_ac_mod in "$1"; do
    if test -n "$_moo_ac_mod"; then
      case "$_moo_ac_mod" in
        testlib)
          _moo_ac_qt_define="QT_TEST_LIB"
          _moo_ac_qt_lib="QtTest"
          ;;
        *)
          AC_MSG_ERROR([Unknown Qt component $_moo_ac_mod])
          ;;
      esac

      QT_CXXFLAGS="$QT_CXXFLAGS -D$_moo_ac_qt_define -I$QT_PREFIX/include/$_moo_ac_qt_lib"
      QT_LIBS="$QT_LIBS -l$_moo_ac_qt_lib"4
    fi
  done

  QT_CXXFLAGS="$QT_CXXFLAGS -DQT_XML_LIB -DQT_SCRIPT_LIB -DQT_GUI_LIB -DQT_CORE_LIB \
-I$QT_PREFIX/include/QtGui -I$QT_PREFIX/include/QtScript \
-I$QT_PREFIX/include/QtCore -I$QT_PREFIX/include/QtXml"
  QT_LIBS="$QT_LIBS -lQtScript4 -lQtXml4 -lQtGui4 -lQtCore4"

  AC_MSG_RESULT(found)
  echo Qt compiler flags: $QT_CXXFLAGS
  echo Qt linker flags: $QT_LIBS
fi
])
