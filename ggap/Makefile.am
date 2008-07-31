include ../mk/pre.mk

bin_PROGRAMS = ggap

ggap_SOURCES =		\
    app.cpp		\
    appwindow.cpp	\
    dialogs.cpp		\
    dialogs.h		\
    gap.cpp		\
    gap.h		\
    gapdoc.cpp		\
    gapdoc.h		\
    gws.cpp		\
    gws.h		\
    help_p.h		\
    help.cpp		\
    main.cpp		\
    parser.cpp		\
    parser.h		\
    parser_p.h		\
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
    help.h		\
    prefsdialog.h	\
    proc.h		\
    proc_p.h		\
    proc-wrapper.h	\
    script.h		\
    windowmenu.h	\
    worksheet.h		\
    worksheet_p.h	\
    wswindow.h		\
    wswindow_p.h

nodist_ggap_SOURCES =	\
    $(moc_gen_cpp)	\
    $(ui_gen_sources)

ggap_LDADD = ../moows/libmoows.a ../contrib/libcontrib.a $(QT_LIBS)
ALL_CXXFLAGS =			\
    -I..			\
    -I$(srcdir)/..		\
    -I$(srcdir)/../contrib	\
    -I$(srcdir)/../contrib/qxt	\
    $(QT_CXXFLAGS)		\
    -Wall -Wreorder		\
    -Wno-non-template-friend	\
    -Woverloaded-virtual	\
    -Wunused

ui_files =			\
    about.ui			\
    help.ui			\
    prefs.ui			\
    wswindow.ui

qrc_files = ggap.qrc gap.qrc
gap_qrc_deps = gap/restore.g gap/ggap.g

include ../mk/qt.mk

pch_header = $(srcdir)/../moows/common.h
include ../mk/pch.mk

%_p.o: %.o
	@true
proc-wrapper.o: proc.o
	@true
# worksheet_p.o: worksheet.o
# proc_p.o: proc.o

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
