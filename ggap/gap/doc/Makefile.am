ggap_docdir = ${GGAP_GAP_DIR}/ggap/doc

msk_files =		\
    types.msk		\
    manual.msk

tex_files =		\
    $(srcdir)/types.tex	\
    $(srcdir)/manual.tex

g_files =			\
    $(srcdir)/../gap/ggap.gd	\
    $(srcdir)/../gap/ggap.gi	\
    $(srcdir)/../gap/gobject.gd	\
    $(srcdir)/../gap/gobject.gi	\
    $(srcdir)/../gap/io.gd	\
    $(srcdir)/../gap/io.gi	\
    $(srcdir)/../gap/override.gd\
    $(srcdir)/../gap/override.gi\
    $(srcdir)/../gap/types.gd	\
    $(srcdir)/../gap/types.gi	\
    $(srcdir)/../gap/utils.gd	\
    $(srcdir)/../gap/utils.gi

dist_ggap_doc_DATA =	\
    manual.pdf		\
    manual.six		\
    $(tex_files)

EXTRA_DIST =		\
    $(msk_files)	\
    genmsk.py		\
    buildman.config.in	\
    buildman.config	\
    buildman.pe		\
    convert.pl		\
    gapmacro.tex	\
    manual.mst		\
    manualindex

buildman.config: buildman.config.in $(top_builddir)/config.status
	cd $(top_builddir) && \
	$(SHELL) ./config.status --file=ggap/gap/doc/buildman.config && \
	cd ggap/gap/doc && mv buildman.config $(srcdir)/

manual.pdf: .pdf-stamp
.pdf-stamp: .manual-stamp
	cd $(srcdir) && \
	pdftex manual && pdftex manual && \
	date > .pdf-stamp

manual.six: .manual-stamp
%.tex: .manual-stamp

types.msk: genmsk.py ../gap/codegen.py ../gap/defs.py ../gap/gtktypes.py
	$(PYTHON) $(srcdir)/genmsk.py $(srcdir)/types.msk

.manual-stamp: $(msk_files) $(g_files) buildman.config
	cd $(srcdir) && \
	./buildman.pe -f buildman.config && \
	tex manual &&  \
	tex manual && \
	./manualindex manual && \
	tex manual && \
	date > .manual-stamp

.htm-stamp: .manual-stamp
	cd $(srcdir) && \
	mkdir -p ../htm && \
	./convert.pl -c -t -n automgrp . ../htm && \
	date > .htm-stamp

CLEANFILES =			\
    $(tex_files)		\
    $(srcdir)/.pdf-stamp	\
    $(srcdir)/.manual-stamp	\
    $(srcdir)/.htm-stamp	\
    $(srcdir)/.tex-stamp	\
    $(srcdir)/manual.aux	\
    $(srcdir)/manual.dvi	\
    $(srcdir)/manual.idx	\
    $(srcdir)/manual.ilg	\
    $(srcdir)/manual.ind	\
    $(srcdir)/manual.lab	\
    $(srcdir)/manual.log	\
    $(srcdir)/manual.pdf	\
    $(srcdir)/manual.six	\
    $(srcdir)/manual.toc	\
    $(srcdir)/notfound		\
    texput.log
