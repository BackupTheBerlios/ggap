# -*- makefile -*-

# $(PCH_HEADER) must be the header file to precompile

EXTRA_DIST += $(PCH_HEADER)

BASE_COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
BASE_CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(ALL_CXXFLAGS) $(CXXFLAGS)

COMPILE = $(BASE_COMPILE)
CXXCOMPILE = $(BASE_CXXCOMPILE)

if MOO_ENABLE_PCH

BUILT_SOURCES += $(pch_files)
CLEANFILES += $(pch_files)

pch_name = precompiled-header-name
pch_dir = $(pch_name).gch
pch_files = $(pch_dir)/c $(pch_dir)/c++

$(pch_dir)/c: $(PCH_HEADER)
	mkdir -p $(pch_dir) && $(BASE_COMPILE) -x c-header -c $< -o $@
$(pch_dir)/c++: $(PCH_HEADER)
	mkdir -p $(pch_dir) && $(BASE_CXXCOMPILE) -x c++-header -c $< -o $@

COMPILE += -include $(pch_name) -Winvalid-pch
CXXCOMPILE += -include $(pch_name) -Winvalid-pch

endif
