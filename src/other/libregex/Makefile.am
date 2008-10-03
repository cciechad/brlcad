# $Id: Makefile.am 32013 2008-07-29 00:50:26Z starseeker $

lib_LTLIBRARIES = libregex.la

libregex_la_SOURCES = \
	regcomp.c \
	regerror.c \
	regexec.c \
	regfree.c

noinst_HEADERS = \
	cclass.h \
	cname.h \
	regex2.h \
	utils.h

include_HEADERS = \
	regex.h

EXTRA_DIST = \
	CMakeLists.txt \
	engine.c

include $(top_srcdir)/misc/Makefile.defs

