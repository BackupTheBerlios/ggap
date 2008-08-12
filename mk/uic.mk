# -*- makefile -*-

##################################################################################
#
# uic
#
# input: QT_UI_FILES - list of .ui files
#

ui_names = $(patsubst %.ui,%,$(QT_UI_FILES))
ui.h.stamp: $(QT_UI_FILES) Makefile $(top_srcdir)/mk/uic-wrapper
	$(top_srcdir)/mk/uic-wrapper $(QT_UIC) $(srcdir) $(ui_names) && \
        echo stamp > ui.h.stamp

EXTRA_DIST += $(QT_UI_FILES)
BUILT_SOURCES += ui.h.stamp
UI_HDRS = $(patsubst %.ui,ui_%.h,$(QT_UI_FILES))
CLEANFILES += $(UI_HDRS) ui.h.stamp
nodist_@MODULE@_SOURCES += $(UI_HDRS)
