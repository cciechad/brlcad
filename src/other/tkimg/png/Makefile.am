# $Id: Makefile.am 30625 2008-04-04 05:40:59Z brlcad $

noinst_LTLIBRARIES = tkimgp.la

# per-program CPPFLAGS wasn't added until automake 1.7
AM_CPPFLAGS = \
	-DPNG_PACKAGE_NAME=\"libpng\" \
	-DPNGTCL_PACKAGE_NAME=\"pngtcl\" \
	-DTKIMG_VERSION="\"${TKIMG_VERSION}\"" \
	-DTKIMG_PNG_PACKAGE_NAME=\"Tkimgpng\" \
	-I${top_srcdir}/src/other/tkimg/base \
	-I${top_srcdir}/src/other/tkimg/png \
	-I${top_srcdir}/src/other/tkimg/pngtcl \
	${PNG_CPPFLAGS} \
	${LIBZ_CPPFLAGS} \
	${TCL_CPPFLAGS} \
	${TK_CPPFLAGS}

AM_CFLAGS = ${X_CFLAGS}

tkimgp_la_LDFLAGS = -module
tkimgp_la_SOURCES = \
	png.c

EXTRA_DIST = \
	aclocal.m4 \
	init.c \
	pkgIndex.tcl.in \
	tests

include $(top_srcdir)/misc/Makefile.defs

