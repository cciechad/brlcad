# $Id: Makefile.am 29060 2007-10-02 23:52:18Z brlcad $

.PHONY : clean clobber run bench

bin_PROGRAMS = pixcmp
pixcmp_SOURCES = pixcmp.c

dist_bin_SCRIPTS = \
	benchmark

man_MANS = \
	benchmark.1 \
	pixcmp.1

EXTRA_DIST = \
	$(man_MANS) \
	${dist_bin_SCRIPTS} \
	lgt.sh \
	run.sh \
	try.sh \
	viewdiff.sh

benchmark: run.sh
	cp $(top_srcdir)/bench/run.sh benchmark

run: all
	@if test -r ${srcdir}/run.sh ; then \
		LD_LIBRARY_PATH=$(top_builddir)/src/other/tcl/unix:$(top_builddir)/src/other/tk/unix:$$LD_LIBRARY_PATH \
		DYLD_LIBRARY_PATH=$(top_builddir)/src/other/tcl/unix:$(top_buildir)/src/other/tk/unix:$$DYLD_LIBRARY_PATH \
		${SH} ${srcdir}/run.sh ; \
	fi
	@$(ECHO) "---"
	@$(ECHO) "Run 'make clean' in the bench directory to delete the log"
	@$(ECHO) "and image files generated during the benchmark analysis."
	@$(ECHO)

bench: run

clean:
	@${SH} ${srcdir}/run.sh clean

clobber:
	@${SH} ${srcdir}/run.sh clobber

CLEANFILES = \
	benchmark

DEPS=src/rt db

include $(top_srcdir)/misc/Makefile.defs
