# $Id: Makefile.am 29063 2007-10-03 19:36:33Z brlcad $

bin_PROGRAMS = \
	canonize \
	pix-ipu \
	png-ipu \
	ipuscan \
	ipustat

AM_CPPFLAGS = ${TCL_CPPFLAGS}

noinst_HEADERS = \
	canon.h \
	chore.h

man_MANS = canonize.1

canonize_SOURCES = canonize.c canonlib.c
canonize_LDADD = ${FB} ${LIBDS}

pix_ipu_SOURCES = pix-ipu.c canonlib.c
pix_ipu_LDADD = ${FB} ${LIBDS}

png_ipu_SOURCES = png-ipu.c canonlib.c
png_ipu_LDADD = ${FB} ${LIBDS}
# per-program CPPFLAGS wasn't added until automake 1.7
png_ipu_CFLAGS = ${PNG_CPPFLAGS}

ipuscan_SOURCES = ipuscan.c canonlib.c
ipuscan_LDADD = ${FB} ${LIBDS}

ipustat_SOURCES = ipustat.c canonlib.c
ipustat_LDADD = ${BU} ${LIBDS}

EXTRA_DIST = $(man_MANS)

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(canonize_OBJECTS) \
	$(canonserver_OBJECTS) \
	$(pix_ipu_OBJECTS) \
	$(png_ipu_OBJECTS) \
	$(ipuscan_OBJECTS) \
	$(ipustat_OBJECTS) \
	$(bin_PROGRAMS)
