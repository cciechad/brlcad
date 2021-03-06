# $Id: Makefile.am 30465 2008-03-05 15:34:29Z erikgreenwald $

lib_LTLIBRARIES = libpng.la

# NOTE: either PNG_NO_MMX_CODE or PNG_NO_ASSEMBLER_CODE are required
# for successful compilation on AMD64 Linux with gcc due to linking of
# 32-bit assembly with 64-bit object files.  Compiling with -m32 also
# avoids the problem.
libpng_la_CFLAGS = ${LIBZ_CPPFLAGS} -DPNG_NO_MMX_CODE
libpng_la_LDFLAGS = -version-info 3:5:2
libpng_la_LIBADD = ${LIBZ}
libpng_la_SOURCES = \
	png.c \
	pngerror.c \
	pnggccrd.c \
	pngget.c \
	pngmem.c \
	pngpread.c \
	pngread.c \
	pngrio.c \
	pngrtran.c \
	pngrutil.c \
	pngset.c \
	pngtrans.c \
	pngvcrd.c \
	pngwio.c \
	pngwrite.c \
	pngwtran.c \
	pngwutil.c

noinst_PROGRAMS = pngtest

pngtest_SOURCES = pngtest.c
pngtest_CFLAGS = ${LIBZ_CPPFLAGS}
pngtest_LDADD = \
	${lib_LTLIBRARIES} \
	${LIBZ} \
	${LIBM}

include_HEADERS = \
	png.h \
	pngconf.h

dist_man_MANS = \
	libpng.3 \
	libpngpf.3 \
	png.5

EXTRA_DIST = \
	ANNOUNCE \
	CHANGES \
	INSTALL \
	KNOWNBUG \
	LICENSE \
	Makefile \
	README \
	TODO \
	Y2KINFO \
	aclocal.m4 \
	autogen.sh \
	config.h.in \
	configure \
	configure.ac \
	contrib \
	example.c \
	libpng-1.2.25.txt \
	pngbar.jpg \
	pngbar.png \
	pngnow.png \
	pngtest.png \
	projects \
	scripts \
	test-pngtest.sh

.PHONY : test
test:	pngtest
	./pngtest ${top_srcdir}/src/other/libpng/pngtest.png

if BUILD_ZLIB
DEPADD = src/other/libz
endif

DEPENDS = ${DEPADD}

include $(top_srcdir)/misc/Makefile.defs

