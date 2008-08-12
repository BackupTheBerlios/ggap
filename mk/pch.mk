# -*- makefile -*-

# $(PCH_HEADER) must be the header file to precompile

EXTRA_DIST += $(PCH_HEADER)

ugly_base_compile_c = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
ugly_base_compile_cxx = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
ugly_base_compile_objc = $(OBJC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_OBJCFLAGS) $(OBJCFLAGS)

COMPILE = $(ugly_base_compile_c)
CXXCOMPILE = $(ugly_base_compile_cxx)
OBJCCOMPILE = $(ugly_base_compile_objc)

if MOO_ENABLE_PCH

BUILT_SOURCES += $(pch_files)
CLEANFILES += $(pch_files)

pch_name = precompiled-header-name
pch_files = $(pch_name)-c.gch $(pch_name)-c++.gch

$(pch_name)-c.gch: $(PCH_HEADER)
	$(ugly_base_compile_c) -x c-header -c $< -o $@
$(pch_name)-c++.gch: $(PCH_HEADER)
	$(ugly_base_compile_cxx) -x c++-header -c $< -o $@

COMPILE += -include $(pch_name)-c -Winvalid-pch
CXXCOMPILE += -include $(pch_name)-c++ -Winvalid-pch

endif
