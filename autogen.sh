#! /bin/sh

builddir=`pwd`

rel_srcdir=`dirname $0`
test -z "$rel_srcdir" && rel_srcdir=.

srcdir=`cd $rel_srcdir && pwd`
echo "srcdir="$srcdir

if test -d $rel_srcdir/m4 ; then
  aclocal_extra="-I m4"
elif test -d $rel_srcdir/moo/m4 ; then
  aclocal_extra="-I moo/m4"
fi

cd $srcdir
echo "Generating configuration files..."

ACLOCAL=${ACLOCAL:-aclocal-1.10}
ACLOCAL_FLAGS="$aclocal_extra $ACLOCAL_FLAGS"
AUTOMAKE=${AUTOMAKE:-automake-1.10}
AUTOHEADER=${AUTOHEADER:-autoheader}
AUTOCONF=${AUTOCONF:-autoconf}
LIBTOOLIZE=${LIBTOOLIZE:-libtoolize}
UGLY=${UGLY:-./mk/ugly}

if [ -f Makefile.ug ]; then
  echo $UGLY
  $UGLY || exit $?
fi

if grep AC_PROG_LIBTOOL configure.ac; then
  echo $LIBTOOLIZE --automake --copy --force
  $LIBTOOLIZE --automake --copy --force || exit $?
fi

if [ -d po ]; then
  echo glib-gettextize --force --copy
  glib-gettextize --force --copy || exit $?
  echo intltoolize --automake --force --copy
  intltoolize --automake --force --copy || exit $?
fi

if [ -d po-gsv ] ; then
  echo sed 's/@GETTEXT_PACKAGE@/@GETTEXT_PACKAGE@-gsv/' po/Makefile.in.in '>' po-gsv/Makefile.in.in
  sed 's/@GETTEXT_PACKAGE@/@GETTEXT_PACKAGE@-gsv/' po/Makefile.in.in > po-gsv/Makefile.in.in || exit $?
fi

echo $ACLOCAL $ACLOCAL_FLAGS
$ACLOCAL $ACLOCAL_FLAGS || exit $?

echo $AUTOHEADER
$AUTOHEADER || exit $?

echo $AUTOMAKE --add-missing --copy
$AUTOMAKE --add-missing --copy || exit $?

echo $AUTOCONF
$AUTOCONF || exit $?

if test -z $1; then
    echo
    echo "run '$rel_srcdir/configure ; make ; make install'"
    echo
else
    if test $1 = "do" -o $1 = "--do"; then
        shift
    fi
    cd $builddir
    $srcdir/configure $*
fi
