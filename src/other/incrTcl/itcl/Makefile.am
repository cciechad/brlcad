# $Id: Makefile.am 29326 2007-11-14 03:50:18Z brlcad $

SUBDIRS = \
	doc \
	generic \
	library

brlcadscriptdir = $(libdir)/itcl@ITCL_VERSION@
brlcadscript_SCRIPTS = pkgIndex.tcl

EXTRA_DIST = \
	$(brlcadscript_SCRIPTS) \
	aclocal.m4 \
	itclConfig.sh.in \
	license.terms \
	tests \
	win

DISTCLEANFILES = pkgIndex.tcl

include $(top_srcdir)/misc/Makefile.defs

