# $Id: Makefile.am 32013 2008-07-29 00:50:26Z starseeker $

SUBDIRS = \
        sfiles

bin_PROGRAMS = \
	nirt \
	showshot

AM_CPPFLAGS = ${TCL_CPPFLAGS}

nirt_SOURCES = \
	nirt.c \
	command.c \
	conversion.c \
	if.c \
	interact.c \
	bsphere.c \
	read_mat.c \
	str_manip.c \
	parse_fmt.c

nirt_CFLAGS = -DDFLT_SHELL=\"/bin/sh\"
nirt_LDADD = ${FB} ${RT}

showshot_SOURCES = showshot.c
showshot_LDADD = ${BN}

man_MANS = \
	nirt.1 \
	showshot.1

noinst_HEADERS = \
	nirt.h \
	usrfmt.h

EXTRA_DIST = \
	$(man_MANS) \
	dist_def.c \
	nirt.dsp

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(nirt_OBJECTS) \
	$(showshot_OBJECTS) \
	$(bin_PROGRAMS)
