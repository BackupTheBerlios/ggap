include ../mk/pre.mk

bin_PROGRAMS = ggap

ggap_SOURCES =		\
    app.cpp		\
    appwindow.cpp	\
    dialogs.cpp		\
    gap.cpp		\
    gap.h		\
    gap-p.h		\
    gws.cpp		\
    gws.h		\
    help-p.h		\
    help-topics.h	\
    help.cpp		\
    help-bookmarks.cpp	\
    main.cpp		\
    parser.cpp		\
    parser.h		\
    parser-p.h		\
    parser-yacc.cpp	\
    parser-yacc.h	\
    prefs.cpp		\
    prefs.h		\
    prefsdialog.cpp	\
    proc.cpp		\
    script.cpp		\
    utils.cpp		\
    utils.h		\
    windowmenu.cpp	\
    worksheet.cpp	\
    wswindow.cpp	\
    $(moc_h_files)	\
    $(moc_cpp_files)	\
    $(qrc_sources)

moc_cpp_files =

moc_h_files =		\
    app.h		\
    appwindow.h		\
    dialogs.h		\
    help.h		\
    help-bookmarks.h	\
    help-bookmarks-p.h	\
    prefsdialog.h	\
    prefsdialog-p.h	\
    proc.h		\
    proc-p.h		\
    proc-wrapper.h	\
    script.h		\
    windowmenu.h	\
    worksheet.h		\
    worksheet-p.h	\
    wswindow.h		\
    wswindow-p.h

nodist_ggap_SOURCES =	\
    $(moc_gen_cpp)	\
    $(ui_gen_sources)

test_sources =		\
    tester.cpp		\
    tester.h
test_moc_h_files =	\
    gws-t.h		\
    parser-t.h		\
    script-t.h
if MOO_ENABLE_UNIT_TESTS
ggap_SOURCES += $(test_sources)
moc_h_files += $(test_moc_h_files)
else
EXTRA_DIST += $(test_sources) $(test_moc_h_files)
endif

ggap_LDADD = ../moows/libmoows.a ../contrib/libcontrib.a $(QT_LIBS)
AM_CXXFLAGS =			\
    -I..			\
    -I$(srcdir)/..		\
    -I$(srcdir)/../contrib	\
    -I$(srcdir)/../contrib/qxt	\
    $(QT_CXXFLAGS)		\
    -Wall -Wreorder		\
    -Wno-non-template-friend	\
    -Woverloaded-virtual	\
    -Wunused -W			\
    $(MOO_UNIT_TESTS_CFLAGS)

ui_files =			\
    about.ui			\
    addbookmark.ui		\
    editdirlist.ui		\
    help.ui			\
    prefs.ui			\
    wswindow.ui

qrc_files = ggap.qrc gap.qrc help.qrc
ggap_qrc_deps = $(top_srcdir)/THANKS
gap_qrc_deps = gap/restore.g gap/ggap.g
help_qrc_deps =			\
    help/prefsdialog.html
EXTRA_DIST += $(help_qrc_deps)

include ../mk/qt.mk

PCH_HEADER = $(srcdir)/../moows/common.h
include ../mk/pch.mk

%-p.o: %.o
	@true
%-t.o: %.o
	@true
proc-wrapper.o: proc.o
	@true

include gap/Makefile.incl

yacc_pp_files = parser-yacc.y
include ../mk/yacc.mk

if MOO_OS_MINGW
# ggap_LDFLAGS += -mwindows
rc_in_files = ggap.rc.in
ggap_LDADD += ggap.res
ggap.exe: ggap.res
include ../mk/rc.mk
endif MOO_OS_MINGW

tests = run-unit-tests
TESTS =
EXTRA_DIST += $(tests)
if MOO_ENABLE_UNIT_TESTS
TESTS += $(tests)
endif
