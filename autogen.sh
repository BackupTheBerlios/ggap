#! /bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

if test "x$LIBTOOLIZE" = "x"; then
    LIBTOOLIZE=libtoolize
fi
if test "x$ACLOCAL" = "x"; then
    ACLOCAL=aclocal
fi
if test "x$AUTOHEADER" = "x"; then
    AUTOHEADER=autoheader
fi
if test "x$AUTOMAKE" = "x"; then
    AUTOMAKE=automake
fi
if test "x$AUTOCONF" = "x"; then
    AUTOCONF=autoconf
fi

echo "Generating GGAP configuration files..."
echo "Adding libtools..."                       && \
$LIBTOOLIZE --automake --copy                   && \
echo "Building macros..."                       && \
$ACLOCAL                                        && \
echo "Building headers..."                      && \
$AUTOHEADER                                     && \
echo "Building makefiles..."                    && \
$AUTOMAKE --add-missing --copy                  && \
echo "Building configure..."                    && \
$AUTOCONF

echo
echo 'run "./configure ; make ; make install"'
echo
