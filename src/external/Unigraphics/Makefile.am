# $Id: Makefile.am 29063 2007-10-03 19:36:33Z brlcad $

# XXX need to build unigraphics conditionally if the dev libs are available
# XXX perhaps make it a special unbuilt-until-requested target
# bin_PROGRAMS = ug-g
# ug_g_SOURCES = ug-g.c ug_misc.c
# ug_g_LDADD = ${WDB} ${UGOPEN} ${TCL}

dist_man_MANS = ug-g.1

EXTRA_DIST = \
	conv.h \
	log.h \
	ug-g.c \
	ug_misc.c \
	ug_misc.h

include $(top_srcdir)/misc/Makefile.defs
