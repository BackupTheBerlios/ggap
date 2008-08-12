# -*- makefile -*-

##################################################################################
#
# qrc
#
# input: QRC_FILES - list of .qrc files
#        QRC_NAME
#        QRC_DEPS
#

qrc_cpp = $(QRC_NAME)-qrc.cpp
$(qrc_cpp): $(QRC_FILES) $(QRC_DEPS)
	$(QT_RCC) -o $(qrc_cpp).tmp -name $(QRC_NAME) $(addprefix $(srcdir)/,$(QRC_FILES)) && mv $(qrc_cpp).tmp $(qrc_cpp)

qrc_names = $(basename $(QRC_FILES))
GEN_QRC_SRCS = $(QRC_FILES:.qrc=-qrc.cpp)

$(foreach name,$(qrc_names),$(eval $(call QRC_template,$(name))))

EXTRA_DIST += $(QRC_FILES) $(QRC_DEPS)
BUILT_SOURCES += $(GEN_QRC_SRCS)
mod_SRCS += $(GEN_QRC_SRCS)
