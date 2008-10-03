# $Id: Makefile.am 32048 2008-07-29 13:14:31Z starseeker $

SUBDIRS = \
	bench \
	doc \
	isst \
	libcommon \
	librender \
	libtexture \
	libtie \
	libtienet \
	libutil \
	master \
	misc \
	rise \
	scripts \
	slave

tieincludedir = $(includedir)/tie
tieinclude_HEADERS = adrt.h adrt_struct.h

if HAVE_DOXYGEN
doxygen:
	mkdir doxygen
	doxygen Doxyfile
endif

EXTRA_DIST = \
	AUTHORS \
	README

DEPS = src/libbu

include $(top_srcdir)/misc/Makefile.defs
