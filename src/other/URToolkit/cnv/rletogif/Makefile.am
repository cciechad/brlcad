# $Id: Makefile.am 29063 2007-10-03 19:36:33Z brlcad $

bin_PROGRAMS = rletogif

rletogif_SOURCES = \
	compgif.c \
	gifencod.c \
	rletogif.c

noinst_HEADERS = rletogif.h

rletogif_LDADD = ${RLE}
rletogif_CFLAGS = ${RLE_CPPFLAGS}

include $(top_srcdir)/misc/Makefile.defs

