# -*- makefile -*-

##################################################################################
#
# moc
#
# input: MOC_HDRS - list of .h files to be moc'ed
#

all-classes-moc.cpp.stamp: $(MOC_HDRS) Makefile $(top_srcdir)/mk/moc-wrapper
	$(top_srcdir)/mk/moc-wrapper $(QT_MOC) $(srcdir) all-classes-moc.cpp $(MOC_HDRS) && echo stamp > all-classes-moc.cpp.stamp

GEN_MOC_SRCS = all-classes-moc.cpp

EXTRA_DIST += $(MOC_HDRS)
BUILT_SOURCES += all-classes-moc.cpp.stamp
CLEANFILES += $(GEN_MOC_SRCS) all-classes-moc.cpp.stamp
nodist_mod_SRCS += $(GEN_MOC_SRCS)
