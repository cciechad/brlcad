# $Id: Makefile.am 32013 2008-07-29 00:50:26Z starseeker $

lib_LTLIBRARIES = libfft.la

noinst_PROGRAMS = \
	fftc \
	fftest \
	ifftc

SRCS = \
	fftfast.c \
	splitdit.c \
	ditsplit.c

libfft_la_SOURCES = \
	$(SRCS) \
	$(BUILT_SOURCES)

libfft_la_LDFLAGS = -version-info 19:1 -no-undefined
libfft_la_LIBADD = \
	${FFT_LIBS}

noinst_HEADERS = \
	fft.h

fftc_LDADD = ${LIBM}
fftc_SOURCES = \
	fftc.c \
	splitditc.c

fftest_LDADD = libfft.la ${LIBM}
fftest_SOURCES = fftest.c

ifftc_LDADD = ${LIBM}
ifftc_SOURCES = \
	ditsplitc.c \
	ifftc.c

rfft16.c:	fftc$(EXEEXT)
	./fftc$(EXEEXT) 16 > rfft16.c

rfft32.c:	fftc$(EXEEXT)
	./fftc$(EXEEXT) 32 > rfft32.c

rfft64.c:	fftc$(EXEEXT)
	./fftc$(EXEEXT) 64 > rfft64.c

rfft128.c:	fftc$(EXEEXT)
	./fftc$(EXEEXT) 128 > rfft128.c

rfft256.c:	fftc$(EXEEXT)
	./fftc$(EXEEXT) 256 > rfft256.c

rfft512.c:	fftc$(EXEEXT)
	./fftc$(EXEEXT) 512 > rfft512.c

rfft1024.c:	fftc$(EXEEXT)
	./fftc$(EXEEXT) 1024 > rfft1024.c

irfft16.c:	ifftc$(EXEEXT)
	./ifftc$(EXEEXT) 16 > irfft16.c

irfft32.c:	ifftc$(EXEEXT)
	./ifftc$(EXEEXT) 32 > irfft32.c

irfft64.c:	ifftc$(EXEEXT)
	./ifftc$(EXEEXT) 64 > irfft64.c

irfft128.c:	ifftc$(EXEEXT)
	./ifftc$(EXEEXT) 128 > irfft128.c

irfft256.c:	ifftc$(EXEEXT)
	./ifftc$(EXEEXT) 256 > irfft256.c

irfft512.c:	ifftc$(EXEEXT)
	./ifftc$(EXEEXT) 512 > irfft512.c

irfft1024.c:	ifftc$(EXEEXT)
	./ifftc$(EXEEXT) 1024 > irfft1024.c

BUILT_SOURCES = \
	irfft16.c \
	irfft32.c \
	irfft64.c \
	irfft128.c \
	irfft256.c \
	rfft16.c \
	rfft32.c \
	rfft64.c \
	rfft128.c \
	rfft256.c

CLEANFILES = $(BUILT_SOURCES)
DISTCLEANFILES = $(CLEANFILES)

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(libfft_la_OBJECTS) \
	$(lib_LTLIBRARIES) \
	$(fftc_OBJECTS) \
	$(fftest_OBJECTS) \
	$(ifftc_OBJECTS) \
	$(bin_PROGRAMS)
