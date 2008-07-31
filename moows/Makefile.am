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
    -Wunused-parameter

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
    wsblock_p.h		\
    wsblock.cpp		\
    wsblockformat.h	\
    wstextblock.cpp	\
    wstextdocument.cpp	\
    $(moc_h_files)	\
    $(moc_cpp_files)	\
    $(moc_gen_cpp)

moc_cpp_files =		\
    worksheet-format.cpp

moc_h_files =		\
    colorcombobox.h	\
    worksheet_p.h	\
    worksheet.h		\
    wstextblock.h	\
    wstextdocument_p.h	\
    wstextdocument.h

include ../mk/qt.mk

pch_header = common.h
include ../mk/pch.mk

%_p.o: %.o
	@true

%-p.o: %.o
	@true
