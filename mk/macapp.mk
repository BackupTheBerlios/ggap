# -*- makefile -*-

if MOO_OS_DARWIN
UGLY_ALL_TARGETS += ugly-mac-app
UGLY_CLEAN_TARGETS += ugly-mac-app-clean
endif

UGLY_MAC_APP = $(MACOSX_APP).app
UGLY_MAC_APP_INFO_PLIST = $($(MACOSX_APP)_INFO_PLIST)
UGLY_MAC_APP_RESOURCES = $($(MACOSX_APP)_APP_RESOURCES)
UGLY_MAC_APP_STAMP = $(UGLY_MAC_APP).stamp

ugly-mac-app-clean:
	rm -rf $(UGLY_MAC_APP) $(UGLY_MAC_APP_STAMP)

ugly-mac-app: $(UGLY_MAC_APP_STAMP)
$(UGLY_MAC_APP_STAMP): $(MACOSX_APP) $(UGLY_MAC_APP_INFO_PLIST) $(UGLY_MAC_APP_RESOURCES)
	rm -rf $(UGLY_MAC_APP) && \
	mkdir -p $(UGLY_MAC_APP)/Contents/MacOS && \
	cp $(MACOSX_APP) $(UGLY_MAC_APP)/Contents/MacOS/ && \
	cp $(UGLY_MAC_APP_INFO_PLIST) $(UGLY_MAC_APP)/Contents/ && \
	(echo 'APPL????' > $(UGLY_MAC_APP)/Contents/PkgInfo) && \
	$(MAKE) $(AM_MAKEFLAGS) ugly-mac-app-copy-data && \
	echo stamp > $(UGLY_MAC_APP_STAMP)

ugly-mac-app-copy-data: ugly-mac-app-copy-resources
ugly-mac-app-copy-resources:
	if [ -n "$(UGLY_MAC_APP_RESOURCES)" ]; then \
	  mkdir -p $(UGLY_MAC_APP)/Contents/Resources/ || exit 1; \
	  cp -R $(UGLY_MAC_APP_RESOURCES) $(UGLY_MAC_APP)/Contents/Resources/ || exit 1; \
	fi
