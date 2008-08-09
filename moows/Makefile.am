include ../mk/pre.mk

noinst_LIBRARIES = libmoows.a

ALL_CXXFLAGS =		\
    -I..		\
    -I$(srcdir)/..	\
    $(QT_CXXFLAGS)	\
    -Wall -Wreorder	\
    -Wno-non-template-friend \
    -Woverloaded-virtual\
    -Wunused -Winit-self\
    -Woverloaded-virtual \
    -Wunused-parameter	\
    -W

libmoows_a_SOURCES =	\
    colorcombobox.cpp	\
    worksheet.cpp	\
    worksheet-draw.cpp	\
    worksheet-file.cpp	\
    worksheet-input.cpp	\
    worksheet-xml.cpp	\
    worksheet-xml.h	\
    worksheet-xml-p.h	\
    wsblock.h		\
    wsblock.cpp		\
    wsblock-p.h		\
    wsblock-format.h	\
    wstextblock.cpp	\
    wstextdocument.cpp	\
    $(moc_h_files)	\
    $(moc_cpp_files)

moc_cpp_files =		\
    worksheet-format.cpp

moc_h_files =		\
    colorcombobox.h	\
    worksheet-p.h	\
    worksheet.h		\
    wstextblock.h	\
    wstextdocument-p.h	\
    wstextdocument.h

nodist_libmoows_a_SOURCES = \
    $(moc_gen_cpp)	\
    $(ui_gen_sources)

include ../mk/qt.mk

pch_header = common.h
include ../mk/pch.mk

wsblock-format.o: wsblock.o
	@true
%_p.o: %.o
	@true
%-p.o: %.o
	@true
