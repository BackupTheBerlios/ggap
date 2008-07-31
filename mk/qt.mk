# -*- makefile -*-

moc_%.cpp.stamp: %.cpp Makefile
	$(QT_MOC) -o moc_$*.cpp.tmp $< && \
        sed 's/\*\* Created: [^*]*$$/** Created: some day/' moc_$*.cpp.tmp > moc_$*.cpp.tmp2 && \
	(cmp -s moc_$*.cpp.tmp2 moc_$*.cpp || mv moc_$*.cpp.tmp2 moc_$*.cpp) && \
	rm -f moc_$*.cpp.tmp moc_$*.cpp.tmp2 && echo stamp > $@

ui_%.h.stamp: $(srcdir)/%.ui Makefile
	$(QT_UIC) -o ui_$*.h.tmp $< && \
        sed 's/\*\* Created: [^*]*$$/** Created: some day/' ui_$*.h.tmp > ui_$*.h.tmp2 && \
        (cmp -s ui_$*.h.tmp2 ui_$*.h || mv ui_$*.h.tmp2 ui_$*.h) && \
        rm -f ui_$*.h.tmp ui_$*.h.tmp2 && echo stamp > $@

moc_gen_cpp = all-classes-moc.cpp
moc_gen_sources = $(if $(moc_h_files),$(moc_gen_cpp))
$(moc_gen_cpp).stamp: $(moc_h_files) Makefile
	(cat $(addprefix $(srcdir)/,$(moc_h_files)) | \
            $(QT_MOC) $(addprefix -f,$(moc_h_files)) -o $(moc_gen_cpp).tmp) && \
        sed 's/\*\* Created: [^*]*$$/** Created: some day/' $(moc_gen_cpp).tmp > $(moc_gen_cpp).tmp2 && \
	(cmp -s $(moc_gen_cpp).tmp2 $(moc_gen_cpp) || mv $(moc_gen_cpp).tmp2 $(moc_gen_cpp)) && \
	rm -f $(moc_gen_cpp).tmp $(moc_gen_cpp).tmp2 && echo stamp > $@

moc_gen_incls = $(patsubst %,moc_%,$(moc_cpp_files))
ui_gen_sources = $(patsubst %.ui,ui_%.h,$(ui_files))

qt_gen_sources = $(moc_gen_sources) $(moc_gen_incls) $(ui_gen_sources)
qt_gen_stamps = $(addsuffix .stamp,$(qt_gen_sources))

EXTRA_DIST += $(moc_h_files) $(ui_files)
BUILT_SOURCES += $(qt_gen_stamps)
CLEANFILES += $(qt_gen_sources) $(qt_gen_stamps)


##################################################################################
#
# qrc
#
# input: qrc_files - list of .qrc files
# output: qrc_sources - list of generated .cpp files
#

define QRC_template
$(1)-qrc.cpp: $(1).qrc $(value $(1)_qrc_deps)
	$(QT_RCC) -o $(1)-qrc.cpp.tmp -name $(1) $(srcdir)/$(1).qrc && mv $(1)-qrc.cpp.tmp $(1)-qrc.cpp
EXTRA_DIST += $(value $(1)_qrc_deps)
endef

qrc_names = $(basename $(qrc_files))
qrc_sources = $(qrc_files:.qrc=-qrc.cpp)

$(foreach name,$(qrc_names),$(eval $(call QRC_template,$(name))))

EXTRA_DIST += $(qrc_files)
BUILT_SOURCES += $(qrc_sources)
