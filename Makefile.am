#
# @(#)$Id: Makefile.am,v 1.14 2005/06/06 14:45:53 emuntyan Exp $
#

DIST_SUBDIRS = pkg ggap m4
SUBDIRS = pkg ggap

ggap_docs =             \
  AUTHORS               \
  ChangeLog             \
  COPYING               \
  NEWS                  \
  README                \
  TODO

EXTRA_DIST =            \
    $(ggap_docs)        \
    makewin             \
    INSTALL             \
    mingw-configure     \
    cygwin-configure    \
    autogen.sh          \
    wininstaller/ggap.iss

ggapdocdir = ${GGAP_DATA_DIR}
ggapdoc_DATA = $(ggap_docs)

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST +=				\
	doc/index.py			\
	doc/parser.py			\
	doc/syms.py			\
	doc/manual/gen_tmpl		\
	doc/manual/html.tmpl		\
	doc/manual/index_html.tmpl	\
	doc/manual/tex.tmpl		\
	doc/manual/ggap_tex.tmpl	\
	doc/manual/Makefile		\
	doc/manual/manual.py		\
	doc/manual/activestrings.tmpl	\
	doc/manual/mooscript.tmpl	\
	doc/manual/usermenu.tmpl
