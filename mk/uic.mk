# -*- makefile -*-

##################################################################################
#
# uic
#
# input: UI_FILES - list of .ui files
#

ui_names = $(patsubst %.ui,%,$(UI_FILES))
ui.h.stamp: $(UI_FILES) Makefile $(top_srcdir)/mk/uic-wrapper
	$(top_srcdir)/mk/uic-wrapper $(QT_UIC) $(srcdir) $(ui_names) && \
        echo stamp > ui.h.stamp

EXTRA_DIST += $(UI_FILES)
BUILT_SOURCES += ui.h.stamp
UI_HDRS = $(patsubst %.ui,ui_%.h,$(UI_FILES))
CLEANFILES += $(UI_HDRS) ui.h.stamp
nodist_mod_SRCS += $(UI_HDRS)
