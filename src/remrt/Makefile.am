# $Id: Makefile.am 29063 2007-10-03 19:36:33Z brlcad $

bin_PROGRAMS = remrt rtsrv

AM_CPPFLAGS = ${TCL_CPPFLAGS}

dist_man_MANS = remrt.1

remrt_LDADD = \
	$(top_builddir)/src/rt/libremrt.a \
	${OPTICAL} \
	${FB}
remrt_SOURCES = \
	ihost.c \
	remrt.c

rtsrv_CFLAGS = -DRTSRV
rtsrv_LDADD = \
	$(top_builddir)/src/rt/libremrt.a \
	${OPTICAL} \
	${PKG} \
	${LIBSOCKET} ${LIBNSL}

rtsrv_SOURCES = \
	rtsrv.c

noinst_HEADERS = ihost.h protocol.h

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(remrt_OBJECTS) \
	$(rtsrv_OBJECTS) \
	$(bin_PROGRAMS)
