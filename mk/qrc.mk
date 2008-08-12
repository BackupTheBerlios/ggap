# -*- makefile -*-

##################################################################################
#
# qrc
#
# input: QT_QRC_FILES - list of .qrc files
#        QT_QRC_NAME
#        QT_QRC_DEPS
#

qrc_cpp = $(QT_QRC_NAME)-qrc.cpp
$(qrc_cpp): $(QT_QRC_FILES) $(QT_QRC_DEPS)
	$(QT_RCC) -o $(qrc_cpp).tmp -name $(QT_QRC_NAME) $(addprefix $(srcdir)/,$(QT_QRC_FILES)) && mv $(qrc_cpp).tmp $(qrc_cpp)

qrc_names = $(basename $(QT_QRC_FILES))
GEN_QRC_SRCS = $(QRC_FILES:.qrc=-qrc.cpp)

$(foreach name,$(qrc_names),$(eval $(call QRC_template,$(name))))

EXTRA_DIST += $(QT_QRC_FILES) $(QT_QRC_DEPS)
BUILT_SOURCES += $(GEN_QRC_SRCS)
mod_SRCS += $(GEN_QRC_SRCS)
