# $Id: Makefile.am 30371 2008-02-19 19:16:16Z erikgreenwald $

noinst_LTLIBRARIES = libvfont_nil.la

libvfont_nil_la_SOURCES = getfont.c

noinst_HEADERS = vfont.h

fontdir = $(BRLCAD_DATA)/vfont
font_DATA = \
	fix.6r \
	nonie.r.12

EXTRA_DIST = $(font_DATA)

AM_CPPFLAGS = ${TCL_CPPFLAGS}

include $(top_srcdir)/misc/Makefile.defs
