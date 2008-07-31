# -*- makefile -*-

pch_name = precompiled-header-name
pch_file = $(pch_name).gch

EXTRA_DIST += $(pch_header)

if MOO_ENABLE_PCH

BUILT_SOURCES += $(pch_file)
DISTCLEANFILES += $(pch_file)

AM_CXXFLAGS = $(ALL_CXXFLAGS) -include $(pch_name) -Winvalid-pch

$(pch_name).gch: $(pch_header)
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(ALL_CXXFLAGS) $(CXXFLAGS) $< -o $@

else

AM_CXXFLAGS = $(ALL_CXXFLAGS)

endif
