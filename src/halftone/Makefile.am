# $Id: Makefile.am 28879 2007-09-07 19:54:19Z erikgreenwald $

bin_PROGRAMS = halftone

AM_CPPFLAGS = ${TCL_CPPFLAGS}

halftone_SOURCES = \
	main.c \
	sharpen.c \
	tone_classic.c \
	tone_floyd.c \
	tone_folly.c \
	tone_simple.c \
	tonescale.c

halftone_LDADD = \
	${RT} ${FB}

man_MANS = halftone.1

EXTRA_DIST = $(man_MANS)

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(halftone_OBJECTS) \
	$(bin_PROGRAMS)
