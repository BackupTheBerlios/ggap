#
# @(#)$Id: Makefile.am,v 1.69 2005/06/06 14:45:54 emuntyan Exp $
#

DIST_SUBDIRS =      \
    moo             \
    gap             \
    sc3             \
    generic

EXTRA_PROGRAMS = ggap
subdirs =
PROFILES =
libs =

if USE_GAP_PROFILE
PROFILES += -DUSE_GAP_PROFILE
subdirs += gap
libs += gap/libgap.la
endif USE_GAP_PROFILE
if USE_SC3_PROFILE
PROFILES += -DUSE_SC3_PROFILE
subdirs += sc3
libs += sc3/libsc3.la
endif USE_SC3_PROFILE
if USE_GENERIC_PROFILE
PROFILES += -DUSE_GENERIC_PROFILE
subdirs += generic
libs += generic/libgeneric.la
endif USE_GENERIC_PROFILE

if !CYGWIN_BUILD
SUBDIRS = moo $(subdirs) .
bin_PROGRAMS = ggap
else CYGWIN_BUILD
SUBDIRS = moo
endif CYGWIN_BUILD

DEFAULT_INCLUDES =
AM_CFLAGS =                     \
    -I$(top_builddir)           \
    -I$(top_builddir)/src/moo   \
    -I$(top_srcdir)/src         \
    $(PROFILES)                 \
    $(MOO_CFLAGS)               \
    $(PYTHON_INCLUDES)          \
    $(PYGTK_CFLAGS)             \
    $(M_CFLAGS)                 \
    $(CFLAGS)
AM_CXXFLAGS =                   \
    -I$(top_builddir)           \
    -I$(top_builddir)/src/moo   \
    -I$(top_srcdir)/src         \
    $(PROFILES)                 \
    $(MOO_CFLAGS)               \
    $(PYTHON_INCLUDES)          \
    $(PYGTK_CFLAGS)             \
    $(M_CXXFLAGS)               \
    $(CXXFLAGS)

ggap_LDADD =                    \
    $(libs)                     \
    moo/libmoo.la               \
    $(MOO_LIBS)                 \
    $(PYTHON_LDFLAGS)           \
    $(PYTHON_EXTRA_LIBS)

if MINGW_BUILD
ggap_LDFLAGS = -mwindows
endif MINGW_BUILD

ggap_SOURCES = main.cpp

nodist_ggap_SOURCES = ggap_res.res
CLEANFILES = ggap_res.res
if MINGW_BUILD
ggap_res.res: ggap_res.rc $(top_srcdir)/pixmaps/ggap.ico
	$(WINDRES) -i ggap_res.rc --input-format=rc -o ggap_res.res -O coff
ggap_SOURCES += ggap_res.res
ggap_LDADD += ggap_res.res
else ! MINGW_BUILD
ggap_res.res: ggap_res.rc $(top_srcdir)/pixmaps/ggap.ico
	touch ggap_res.res
endif ! MINGW_BUILD

EXTRA_DIST = ggap_res.rc.in
