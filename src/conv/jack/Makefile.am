# $Id: Makefile.am 30666 2008-04-13 22:02:16Z brlcad $

bin_PROGRAMS = g-jack jack-g

AM_CPPFLAGS = ${TCL_CPPFLAGS}

g_jack_SOURCES = g-jack.c
g_jack_LDADD = \
	${WDB} \
	${WDB_LIBS}

jack_g_SOURCES = jack-g.c
jack_g_LDADD = \
	${WDB} \
	${WDB_LIBS}

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(g_jack_OBJECTS) \
	$(jack_g_OBJECTS) \
	$(bin_PROGRAMS)
