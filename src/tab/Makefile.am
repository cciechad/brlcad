# $Id: Makefile.am 30740 2008-04-19 15:08:50Z brlcad $

if WITH_PARSERS
SS_BIN = scriptsort
SS_SRC = scriptsort.c script.l
SS_BLT = script.c
else
SS_BIN =
SS_SRC =
SS_BLT =
endif

bin_PROGRAMS = \
	$(SS_BIN) \
	script-tab \
	tabinterp \
	tabsub \
	txyz-pl

AM_CPPFLAGS = ${TCL_CPPFLAGS}
LDADD = ${RT}

scriptsort_SOURCES = $(SS_SRC)
noinst_HEADERS = tokens.h

man_MANS = \
	scriptsort.1 \
	tabinterp.1 \
	tabsub.1

BUILT_SOURCES = ${SS_BLT}

DISTCLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	$(man_MANS) \
	script.l \
	scriptsort.c

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(tabinterp_OBJECTS) \
	$(txyz_pl_OBJECTS) \
	$(tabsub_OBJECTS) \
	$(scriptsort_OBJECTS) \
	$(script_tab_OBJECTS) \
	$(bin_PROGRAMS)
