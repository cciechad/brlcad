# $Id: Makefile.am 32171 2008-07-31 22:44:57Z starseeker $

lib_LTLIBRARIES = libtclcad.la

libtclcad_la_SOURCES = \
	tclcad.c \
	tclcadAutoPath.c \
	tclcadTkSetup.c \
	tkImgFmtPIX.c

libtclcad_la_CFLAGS = \
	${X_CFLAGS} \
	${ITK_CPPFLAGS} \
	${ITCL_CPPFLAGS} \
	${TK_CPPFLAGS} \
	${TCL_CPPFLAGS} \
	-I$(top_srcdir)/src/other/tcl/generic \
	-I$(top_srcdir)/src/other/tcl/unix \
	-I$(top_srcdir)/src/other/tk/generic \
	-I$(top_srcdir)/src/other/tk/unix \
	-I$(top_srcdir)/src/other/blt/src
libtclcad_la_LDFLAGS = -version-info 19:1 -no-undefined
libtclcad_la_LIBADD = ${TCLCAD_LIBS}

EXTRA_DIST = libtclcad.dsp

DEPS=src/libfb src/other/blt

include $(top_srcdir)/misc/Makefile.defs
