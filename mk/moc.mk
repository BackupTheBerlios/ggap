# -*- makefile -*-

##################################################################################
#
# moc
#
# input: QT_MOC_HDRS - list of .h files to be moc'ed
#

%.moc.stamp: $(QT_MOC_HDRS) Makefile $(top_srcdir)/mk/moc-wrapper
	$(top_srcdir)/mk/moc-wrapper $(QT_MOC) $(srcdir) $*.cpp $(QT_MOC_HDRS) && echo stamp > $*.moc.stamp

ugly_moc_sources = all-classes-moc.cpp
ugly_moc_stamps = $(ugly_moc_sources:.cpp=.moc.stamp)

EXTRA_DIST += $(QT_MOC_HDRS)
BUILT_SOURCES += $(ugly_moc_stamps)
CLEANFILES += $(ugly_moc_sources) $(ugly_moc_stamps)
nodist_@MODULE@_SOURCES += $(ugly_moc_sources)
