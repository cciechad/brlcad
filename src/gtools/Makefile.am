# $Id: Makefile.am 32013 2008-07-29 00:50:26Z starseeker $
SUBDIRS = beset
bin_PROGRAMS = \
	g_diff \
	g_lint \
	g_qa \
	solshoot \
	testfree

noinst_PROGRAMS = \
	g_transfer

AM_CPPFLAGS = ${TCL_CPPFLAGS}

g_diff_SOURCES = g_diff.c
g_diff_LDADD = ${RT} ${GED}

g_transfer_SOURCES = g_transfer.c
g_transfer_LDADD = ${RT} ${PKG}

g_lint_SOURCES = g_lint.c
g_lint_LDADD = ${RT}

g_qa_SOURCES = g_qa.c
g_qa_LDADD = ${RT}

solshoot_SOURCES = solshoot.c
solshoot_LDADD = ${RT}

testfree_SOURCES = testfree.c
testfree_LDADD = ${RT}

man_MANS = \
	g_diff.1 \
	g_lint.1 \
	g_qa.1

EXTRA_DIST = $(man_MANS)

sample_applicationsdir=$(SAMPLE_APPLICATIONS_DIR)
sample_applications_DATA = g_transfer.c

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(g_lint_OBJECTS) \
	$(g_diff_OBJECTS) \
	$(solshoot_OBJECTS) \
	$(testfree_OBJECTS) \
	$(noinst_PROGRAMS) \
	$(bin_PROGRAMS)
