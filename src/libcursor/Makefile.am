# $Id: Makefile.am 32013 2008-07-29 00:50:26Z starseeker $

lib_LTLIBRARIES = libcursor.la

man_MANS = libcursor.3

libcursor_la_SOURCES = cursor.c

libcursor_la_LDFLAGS = -version-info 19:1 -no-undefined
libcursor_la_LIBADD = ${CURSOR_LIBS}
libcursor_la_CFLAGS = ${TERMLIB_CPPFLAGS}

EXTRA_DIST = $(man_MANS)

include $(top_srcdir)/misc/Makefile.defs
