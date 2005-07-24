#
# @(#)$Id: Makefile.am,v 1.2 2005/05/29 01:52:37 emuntyan Exp $
#

DEFAULT_INCLUDES =
AM_CFLAGS =                 \
    -I$(top_builddir)       \
    -I$(top_srcdir)/src     \
    $(MOOEDIT_CFLAGS)       \
    $(CFLAGS)               \
    $(M_CFLAGS)
AM_CXXFLAGS =               \
    -I$(top_builddir)       \
    -I$(top_builddir)/src   \
    -I$(top_srcdir)/src     \
    $(MOOEDIT_CFLAGS)       \
    $(PYTHON_INCLUDES)      \
    $(CXXFLAGS)             \
    $(M_CXXFLAGS)

noinst_LTLIBRARIES = libgeneric.la

libgeneric_la_SOURCES = dummy.cpp
