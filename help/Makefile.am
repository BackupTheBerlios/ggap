moo_doc_dir = $(top_srcdir)/moo/doc

docbook_files =						\
	ggap.docbook					\
	$(moo_doc_dir)/editor-editing.docbook		\
	$(moo_doc_dir)/editor-open-save.docbook		\
	$(moo_doc_dir)/editor-plugins.docbook		\
	$(moo_doc_dir)/editor-prefs.docbook		\
	$(moo_doc_dir)/editor-tools.docbook		\
	$(moo_doc_dir)/license.docbook

help_css = $(moo_doc_dir)/help.css
help_xsl = $(moo_doc_dir)/help.xsl
manual_xsl = $(moo_doc_dir)/manual.xsl
txt2xml = $(moo_doc_dir)/txt2xml

EXTRA_DIST =			\
	ggap.docbook

all: build-help manual.html

build-help: help/help.css

DISTCLEANFILES = LGPL.xml
# Stupid form feed characters and stupid XML and stupid xsltproc!!!
LGPL.xml: $(top_srcdir)/COPYING $(txt2xml)
	$(txt2xml) $(top_srcdir)/COPYING > LGPL.xml.tmp && \
	mv LGPL.xml.tmp LGPL.xml

stamp-help: $(docbook_files) $(help_xsl) LGPL.xml
	rm -f help/*.html && \
	$(mkdir_p) help && \
	xsltproc --xinclude --path ".:$(top_srcdir)/moo/doc" --stringparam base.dir $(builddir)/help/ $(help_xsl) $(srcdir)/ggap.docbook && \
	echo stamp > stamp-help
help/help.css: stamp-help $(help_css)
	cp $(help_css) help/

manual.html: $(docbook_files) $(manual_xsl) LGPL.xml
	xsltproc --xinclude --path ".:$(top_srcdir)/moo/doc" --output manual.html $(manual_xsl) $(srcdir)/ggap.docbook

#############################################################################
# Standard automake stuff
#
install: install-data
install-data: build-help manual.html
	$(mkdir_p) $(DESTDIR)$(htmldir)/help
	$(INSTALL_DATA) manual.html $(DESTDIR)$(htmldir)/
	$(INSTALL_DATA) help/* $(DESTDIR)$(htmldir)/help/
uninstall:
	rm -fr $(DESTDIR)$(htmldir)/help $(htmldir)/manual.html

clean: mostlyclean
mostlyclean:
	rm -fr help stamp-help manual.html
distclean: clean
	rm -f Makefile Makefile.in
