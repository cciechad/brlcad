# $Id: Makefile.am 32013 2008-07-29 00:50:26Z starseeker $

# XXX uncomment after 7.8.4
# lib_LTLIBRARIES = libbrlcad.la
noinst_LTLIBRARIES = libbrlcad_nil.la

# libbrlcad_la_SOURCES =
# libbrlcad_la_LDFLAGS = -version-info 19:1
# libbrlcad_la_LIBADD = \
#	libbrlcad_nil.la

if BUILD_OPENNURBS
ON_NIL = $(top_builddir)/src/other/openNURBS/libopenNURBS_nil.la
else
ON_NIL =
endif

libbrlcad_nil_la_SOURCES =
libbrlcad_nil_la_LIBADD = \
	$(top_builddir)/src/libbu/libbu_nil.la \
	$(top_builddir)/src/libbn/libbn_nil.la \
	$(top_builddir)/src/librt/librt_nil.la \
	$(top_builddir)/src/libwdb/libwdb_nil.la \
	$(top_builddir)/src/libged/libged_nil.la \
	${ON_NIL} \
	${TCL}

required_dirs = \
	other \
	libbu \
	libbn \
	libsysv \
	librt \
	external

rts_dirs = librtserver

bench_dirs = \
	libpkg \
	libfb \
	libwdb \
	libged \
	liboptical \
	liborle \
	conv \
	fbserv \
	rt

# anything not included from above
kitchensink_dirs = \
	libcursor \
	libdm \
	libfft \
	libmultispectral \
	libtclcad \
	libtermio \
	adrt \
	anim \
	archer \
	brlman \
	burst \
	bwish \
	canon \
	fb \
	vfont \
	fbed \
	gtools \
	halftone \
	irprep \
	java \
	lgt \
	mged \
	mk \
	nirt \
	proc-db \
	remrt \
	rttherm \
	sig \
	tab \
	tclscripts \
	util \
	vas4 \
	vdeck

#######################
# put it all together #
#######################

if ONLY_BENCHMARK

if ONLY_RTS
SUBDIRS = \
	$(required_dirs) \
	$(bench_dirs) \
	$(rts_dirs)

else !ONLY_RTS
SUBDIRS = \
	$(required_dirs) \
	$(bench_dirs)

endif
# end ONLY_RTS

else !ONLY_BENCHMARK

if ONLY_RTS
SUBDIRS = \
	$(required_dirs) \
	$(rts_dirs)

else !ONLY_RTS
SUBDIRS = \
	$(required_dirs) \
	$(bench_dirs) \
	$(rts_dirs) \
	$(kitchensink_dirs)

endif
# end ONLY_RTS

endif
# end ONLY_BENCHMARK

DIST_SUBDIRS= \
	$(required_dirs) \
	$(bench_dirs) \
	$(rts_dirs) \
	$(kitchensink_dirs)

EXTRA_DIST = README

include $(top_srcdir)/misc/Makefile.defs
