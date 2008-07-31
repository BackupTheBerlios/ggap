include ../mk/pre.mk

noinst_LIBRARIES = libcontrib.a

libcontrib_a_SOURCES =

AM_CXXFLAGS =			\
    $(QT_CXXFLAGS)		\
    -I$(srcdir)/qxt

include ../mk/qt.mk


libcontrib_a_SOURCES +=		\
    qxt/qxtcommandoptions.cpp	\
    qxt/qxtcommandoptions.h	\
    qxt/qxtglobal.h		\
    qxt/qxtpimpl.h

EXTRA_DIST +=			\
    qxt/AUTHORS			\
    qxt/LICENSE			\
    qxt/README


libcontrib_a_SOURCES +=		\
    zip/unzip_p.h		\
    zip/unzip.cpp		\
    zip/unzip.h			\
    zip/zip_p.h			\
    zip/zip.cpp			\
    zip/zip.h			\
    zip/zipentry_p.h

EXTRA_DIST +=			\
    zip/README
