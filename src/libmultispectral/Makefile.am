# $Id: Makefile.am 32013 2008-07-29 00:50:26Z starseeker $

lib_LTLIBRARIES = libmultispectral.la

AM_CPPFLAGS = \
	-DRT_MULTISPECTRAL \
	${TCL_CPPFLAGS}

libmultispectral_la_LDFLAGS = -version-info 19:1 -no-undefined
libmultispectral_la_LIBADD = \
	${top_builddir}/src/liboptical/librtms.la \
	${MULTISPECTRAL_LIBS}

libmultispectral_la_SOURCES = \
	init.c \
	sh_temp.c

include $(top_srcdir)/misc/Makefile.defs
