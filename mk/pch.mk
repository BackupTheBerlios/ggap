# -*- makefile -*-

# $(PCH_HEADER) must be the header file to precompile

EXTRA_DIST += $(PCH_HEADER)

BASE_COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
BASE_CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)

COMPILE = $(BASE_COMPILE)
CXXCOMPILE = $(BASE_CXXCOMPILE)

if MOO_ENABLE_PCH

BUILT_SOURCES += $(pch_files)
CLEANFILES += $(pch_files)

pch_name = precompiled-header-name
pch_files = $(pch_name)-c.gch $(pch_name)-c++.gch

$(pch_name)-c.gch: $(PCH_HEADER)
	$(BASE_COMPILE) -x c-header -c $< -o $@
$(pch_name)-c++.gch: $(PCH_HEADER)
	$(BASE_CXXCOMPILE) -x c++-header -c $< -o $@

COMPILE += -include $(pch_name)-c -Winvalid-pch
CXXCOMPILE += -include $(pch_name)-c++ -Winvalid-pch

endif
