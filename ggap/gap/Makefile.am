SUBDIRS = doc

ggap_pkgdir = ${GGAP_GAP_DIR}/ggap
ggap_pkg_gapdir = ${GGAP_GAP_DIR}/ggap/gap
ggap_pkg_tstdir = ${GGAP_GAP_DIR}/ggap/tst
ggap_pkg_tst_gladedir = ${GGAP_GAP_DIR}/ggap/tst/glade
ggap_pkg_tst_demodir = ${GGAP_GAP_DIR}/ggap/tst/demo

ggap_pkg_DATA =			\
	PackageInfo.g		\
	init.g			\
	read.g

ggap_pkg_gap_DATA =		\
	gap/ggap.gd		\
	gap/ggap.gi		\
	gap/gobject.gd		\
	gap/gobject.gi		\
	gap/graph.gd		\
	gap/graph.gi		\
	gap/io.gd		\
	gap/io.gi		\
	gap/override.gd		\
	gap/override.gi		\
	gap/utils.gd		\
	gap/utils.gi		\
	gap/types.gd		\
	gap/types.gi

ggap_pkg_tst_DATA =		\
	tst/demo.g		\
	tst/testglade.g		\
	tst/treemodel.g

ggap_pkg_tst_glade_DATA =	\
	tst/glade/demo.glade	\
	tst/glade/file1.glade

ggap_pkg_tst_demo_DATA =	\
	tst/demo/message.g	\
	tst/demo/entries.g	\
	tst/demo/filechooser.g

py_files =			\
	gap/codegen.py		\
	gap/defs.py		\
	gap/gtktypes.py

BUILT_SOURCES =			\
	gap/types.gi		\
	gap/types.gd

generate: ; $(PYTHON) $(srcdir)/gap/codegen.py $(srcdir)/gap
$(srcdir)/gap/types.gd: $(py_files) ; $(MAKE) generate
$(srcdir)/gap/types.gi: $(py_files) ; $(MAKE) generate

CLEANFILES =

EXTRA_DIST =			\
	$(ggap_pkg_DATA)	\
	$(ggap_pkg_gap_DATA)	\
	$(ggap_pkg_tst_DATA)	\
	$(ggap_pkg_tst_glade_DATA)\
	$(ggap_pkg_tst_demo_DATA)\
	$(py_files)
