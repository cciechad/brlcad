# $Id: Makefile.am 30223 2008-02-05 20:19:34Z erikgreenwald $

bin_PROGRAMS = adrt_master

adrt_master_SOURCES = \
	compnet.c \
	dispatcher.c \
	master.c \
	main.c

adrt_master_CFLAGS = \
	-I${top_srcdir}/src/adrt \
	-I${top_srcdir}/src/adrt/libtie \
	-I${top_srcdir}/src/adrt/libtienet \
	-I${top_srcdir}/src/adrt/libtexture \
	-I${top_srcdir}/src/adrt/librender \
	${TCL_CPPFLAGS} \
	${LIBZ_CPPFLAGS}

adrt_master_LDADD = \
	${top_builddir}/src/adrt/libtienet/libtienet.la \
	${LIBTHREAD} \
	${LIBM} \
	${LIBZ}

noinst_HEADERS = \
	compnet.h \
	dispatcher.h \
	master.h

include $(top_srcdir)/misc/Makefile.defs
