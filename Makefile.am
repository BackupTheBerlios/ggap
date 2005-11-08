#
# @(#)$Id: Makefile.am,v 1.14 2005/06/06 14:45:53 emuntyan Exp $
#

DIST_SUBDIRS = pkg ggap wininstaller
SUBDIRS = pkg ggap

ggap_docs =             \
  AUTHORS               \
  ChangeLog             \
  COPYING               \
  NEWS                  \
  README                \
  TODO

EXTRA_DIST = $(ggap_docs) makewin INSTALL mingw-configure cygwin-configure autogen.sh

ggapdocdir = ${GGAP_ROOT_DIR}
ggapdoc_DATA = $(ggap_docs)

ACLOCAL_AMFLAGS = -I m4
