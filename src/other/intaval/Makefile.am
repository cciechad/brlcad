# $Id: Makefile.am 29214 2007-10-31 04:48:47Z brlcad $

EXTRA_PROGRAMS = tgf-g

AM_CPPFLAGS = ${TCL_CPPFLAGS} ${OPENNURBS_CPPFLAGS}

tgf_g_SOURCES = \
	read_dra.cpp \
	regtab.cpp \
	tgf-g.cpp \
	write_brl.cpp

tgf_g_LDADD = ${WDB}

noinst_HEADERS = glob.h \
	read_dra.h \
	regtab.h \
	write_brl.h

EXTRA_DIST = readme.txt \
	tgf2g.dsp

DEPS=src/libwdb src/librt

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(tgf_g_OBJECTS) \
	$(bin_PROGRAMS)
