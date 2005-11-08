all-before: ../ggap/moo/mooutils/moomarshals.h ../ggap/moo/mooutils/moomarshals.c ../ggap/ggap.c

../ggap/moo/mooutils/moomarshals.h: ../ggap/moo/mooutils/moomarshals.list
	glib-genmarshal --prefix=_moo_marshal --header ../ggap/moo/mooutils/moomarshals.list > ../ggap/moo/mooutils/moomarshals.h

../ggap/moo/mooutils/moomarshals.c: ../ggap/moo/mooutils/moomarshals.list
	glib-genmarshal --prefix=_moo_marshal --body ../ggap/moo/mooutils/moomarshals.list > ../ggap/moo/mooutils/moomarshals.c

../ggap/ggap.c: ../ggap/ggap.opag
	opag -f _ggap_parse_options -O _ggap_opt_ -A _ggap_arg_ ../ggap/ggap.opag ../ggap/ggap.c
