# $Id: Makefile.am 28879 2007-09-07 19:54:19Z erikgreenwald $

bin_PROGRAMS = fbserv

AM_CPPFLAGS = ${TCL_CPPFLAGS}

fbserv_LDADD = ${FB}

fbserv_SOURCES = \
	fbserv.c \
	server.c

man_MANS = fbserv.1
EXTRA_DIST = \
	$(man_MANS)

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(fbserv_OBJECTS) \
	$(bin_PROGRAMS)
