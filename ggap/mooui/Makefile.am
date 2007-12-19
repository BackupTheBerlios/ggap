noinst_LTLIBRARIES = libmooui.la
noinst_PROGRAMS = testui

libmooui_la_SOURCES =			\
	mdapp.c				\
	mdapp.h				\
	mdasyncop.c			\
	mdasyncop.h			\
	mddialogs.c			\
	mddialogs.h			\
	mddocument.c			\
	mddocument.h			\
	mddocument-private.h		\
	mdfileop.c			\
	mdfileop.h			\
	mdtypes.h			\
	mdutils.c			\
	mdutils.h			\
	mdview.c			\
	mdview.h			\
	mdview-private.h		\
	mdwindow.c			\
	mdwindow.h			\
	mdwindow-private.h		\
	mdmanager.c			\
	mdmanager.h

mdview-private.lo: mdview.lo
mddocument-private.lo: mddocument.lo
mdwindow-private.lo: mdwindow.lo
mdmanager-private.lo: mdmanager.lo

testui_LDADD = libmooui.la $(MOO_LIBS)

AM_CFLAGS =				\
    -I$(top_builddir)			\
    -I$(top_builddir)/ggap		\
    -I$(top_srcdir)/ggap		\
    $(MOO_CFLAGS)			\
    $(MOO_DEBUG_CFLAGS)			\
    $(GTK_CFLAGS)			\
    -DGGAP_LOCALE_DIR=\"$(datadir)/locale\"
AM_OBJCFLAGS = $(AM_CFLAGS) $(MOO_OBJCFLAGS)

EXTRA_DIST = marshals.list

BUILT_SOURCES =				\
	marshals.h			\
	mdenums.h			\
	md-generated.c
nodist_libmooui_la_SOURCES =		\
	marshals.h			\
	mdenums.h			\
	md-generated.c
CLEANFILES =				\
	marshals.h			\
	mdenums.h			\
	md-generated.c			\
	stamp-md-generated.c		\
	stamp-mdenums.h

marshals.h: marshals.list
	glib-genmarshal --prefix=_moo_ui_marshal --header	\
	       $(srcdir)/marshals.list > $@.tmp &&		\
	mv $@.tmp $@

mdenums_headers =			\
	mdapp.h				\
	mdasyncop.h			\
	mddialogs.h			\
	mddocument.h			\
	mdfileop.h			\
	mdmanager.h			\
	mdtypes.h			\
	mdutils.h			\
	mdview.h			\
	mdwindow.h

mdenums.h: stamp-mdenums.h
	@true
md-generated.c: stamp-md-generated.c
	@true
stamp-mdenums.h: $(mdenums_headers) Makefile
	( cd $(srcdir) && glib-mkenums \
			--fhead "#ifndef MD_ENUMS_H\n#define MD_ENUMS_H\n\n#include <glib-object.h>\n" \
			--fhead "\nG_BEGIN_DECLS\n\n\n" \
			--fprod "/* enumerations from \"@filename@\" */\n\n" \
			--vhead "GType @enum_name@_get_type (void) G_GNUC_CONST;\n#define MD_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n\n" \
			--ftail "\nG_END_DECLS\n\n#endif /* MD_ENUMS_H */" \
		$(mdenums_headers) ) > mdenums.h.tmp \
	&& (cmp -s mdenums.h.tmp mdenums.h || cp mdenums.h.tmp mdenums.h ) \
	&& rm -f mdenums.h.tmp \
	&& echo stamp > $(@F)
stamp-md-generated.c: $(mdenums_headers) marshals.list Makefile
	( cd $(srcdir) && glib-mkenums \
			--fhead "#include \"mdenums.h\"\n\n" \
			--fprod "#include \"@filename@\"\n\n" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n    static GType type;\n\n    if (G_UNLIKELY (!type))\n    {\n        static const G@Type@Value values[] = {" \
			--vprod "            { @VALUENAME@, (char*) \"@VALUENAME@\", (char*) \"@valuenick@\" }," \
			--vtail "            { 0, NULL, NULL }\n        };\n\n        type = g_@type@_register_static (\"@EnumName@\", values);\n    }\n\n    return type;\n}\n\n" \
		$(mdenums_headers) ) > md-generated.c.tmp \
	&& echo '#include "marshals.h"' >> md-generated.c.tmp \
	&& glib-genmarshal --prefix=_moo_ui_marshal --body $(srcdir)/marshals.list >> md-generated.c.tmp \
	&& (cmp -s md-generated.c.tmp md-generated.c || cp md-generated.c.tmp md-generated.c ) \
	&& rm -f md-generated.c.tmp \
	&& echo stamp > $(@F)

splint:
	$(srcdir)/run-splint.sh $(srcdir) $(nodist_libmooui_la_SOURCES) $(libmooui_la_SOURCES) --opts $(AM_CFLAGS)
splint-%.c: %.c
	$(srcdir)/run-splint.sh $(srcdir) $< --opts $(AM_CFLAGS)
