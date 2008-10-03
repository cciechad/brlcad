# $Id: Makefile.am 29063 2007-10-03 19:36:33Z brlcad $

bin_PROGRAMS = vdeck

AM_CPPFLAGS = ${TCL_CPPFLAGS}

vdeck_LDADD = ${RT}
vdeck_SOURCES = \
	cgarbs.c \
	match.c \
	parsarg.c \
	vdeck.c

noinst_HEADERS = std.h vextern.h

man_MANS = vdeck.1

EXTRA_DIST = \
	$(man_MANS) \
	vproc.c

include $(top_srcdir)/misc/Makefile.defs
