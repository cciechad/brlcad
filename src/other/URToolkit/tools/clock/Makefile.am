# $Id: Makefile.am 29063 2007-10-03 19:36:33Z brlcad $

bin_PROGRAMS = rleClock

rleClock_SOURCES = \
	font.c \
	rleClock.c

noinst_HEADERS = font.h

font.c: ${srcdir}/makeFont ${srcdir}/font.src
	@SH@ ${srcdir}/makeFont ${srcdir}/font.src > font.c

rleClock_LDADD = ${RLE}
rleClock_CFLAGS = ${RLE_CPPFLAGS}

EXTRA_DIST = \
	README \
	font.src \
	makeFont

include $(top_srcdir)/misc/Makefile.defs

