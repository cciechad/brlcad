# $Id: Makefile.am 30359 2008-02-19 15:00:11Z erikgreenwald $

bin_PROGRAMS = lgt

AM_CPPFLAGS = ${TCL_CPPFLAGS}

lgt_SOURCES = \
	char.c \
	do_options.c \
	error.c \
	execshell.c \
	fb.c \
	glob.c \
	grid_rotate.c \
	hmenu.c \
	ir.c \
	lgt.c \
	lgt_db.c \
	mat_db.c \
	octree.c \
	prnt.c \
	reflect.c \
	sgi_dep.c \
	texture.c \
	trie.c

noinst_HEADERS = \
	ascii.h \
	cursors.h \
	extern.h \
	hmenu.h \
	lgt.h \
	mat_db.h \
	screen.h \
	texture.h \
	tree.h \
	vecmath.h

lgt_LDADD = \
	$(top_builddir)/src/vfont/libvfont_nil.la \
	${RT} \
	${FB} \
	${TERMIO} \
	${CURSOR} \
	${LIBSGIGL}

lgt_CFLAGS = -DBEHIND_ME_TOL=0.05

dist_man_MANS = lgt.1

DEPENDS = src/librt src/libfb src/vfont

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(lgt_OBJECTS) \
	$(bin_PROGRAMS)
