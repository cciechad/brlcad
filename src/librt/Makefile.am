# $Id: Makefile.am 32048 2008-07-29 13:14:31Z starseeker $


lib_LTLIBRARIES = librt.la
noinst_LTLIBRARIES = \
	librt_nil.la \
	librt_xxx.la

BREP_TEST = brep_test
BREP_CPPFLAGS = @SSE@ -DOBJ_BREP=1 ${TNT_CPPFLAGS} ${OPENNURBS_CPPFLAGS}
BREP_LIBADD = ${OPENNURBS}

brep_test_SOURCES = brep_test.cpp
# cannot set per-target CPPFLAGS until automake 1.7+
brep_test_CXXFLAGS = ${SSE}
brep_test_LDADD = \
	librt.la \
	${RT_LIBS}

# cannot set per-target CPPFLAGS until automake 1.7+
AM_CPPFLAGS = \
	${REGEX_CPPFLAGS} \
	${TCL_CPPFLAGS} \
	${BREP_CPPFLAGS}

noinst_PROGRAMS = \
	comb \
	nurb_example \
	${BREP_TEST}

# make sure the example compiles, but don't include it in librt
librt_xxx_la_SOURCES = primitives/xxx/xxx.c

librt_nil_la_SOURCES = \
	bezier_2d_isect.c \
	binary_obj.c \
	bomb.c \
	bool.c \
	bundle.c \
	cmd.c \
	cut.c \
	db5_alloc.c \
	db5_bin.c \
	db5_comb.c \
	db5_io.c \
	db5_scan.c \
	db5_types.c \
	db_alloc.c \
	db_anim.c \
	db_comb.c \
	db_flags.c \
	db_inmem.c \
	db_io.c \
	db_lookup.c \
	db_match.c \
	db_open.c \
	db_path.c \
	db_scan.c \
	db_tree.c \
	db_walk.c \
	dir.c \
	dspline.c \
	fortray.c \
	globals.c \
	htbl.c \
	many.c \
	mater.c \
	memalloc.c \
	mirror.c \
	mkbundle.c \
	opennurbs_ext.cpp \
	oslo_calc.c \
	oslo_map.c \
	pr.c \
	prep.c \
	primitives/arb8/arb8.c \
	primitives/arbn/arbn.c \
	primitives/ars/ars.c \
	primitives/bot/bot.c \
	primitives/brep/brep.cpp \
	primitives/bspline/bspline.c \
	primitives/bspline/nurb_basis.c \
	primitives/bspline/nurb_bezier.c \
	primitives/bspline/nurb_bound.c \
	primitives/bspline/nurb_c2.c \
	primitives/bspline/nurb_copy.c \
	primitives/bspline/nurb_diff.c \
	primitives/bspline/nurb_eval.c \
	primitives/bspline/nurb_flat.c \
	primitives/bspline/nurb_interp.c \
	primitives/bspline/nurb_knot.c \
	primitives/bspline/nurb_norm.c \
	primitives/bspline/nurb_plot.c \
	primitives/bspline/nurb_poly.c \
	primitives/bspline/nurb_ray.c \
	primitives/bspline/nurb_refine.c \
	primitives/bspline/nurb_reverse.c \
	primitives/bspline/nurb_solve.c \
	primitives/bspline/nurb_split.c \
	primitives/bspline/nurb_tess.c \
	primitives/bspline/nurb_trim.c \
	primitives/bspline/nurb_trim_util.c \
	primitives/bspline/nurb_util.c \
	primitives/bspline/nurb_xsplit.c \
	primitives/cline/cline.c \
	primitives/dsp/dsp.c \
	primitives/ebm/ebm.c \
	primitives/ehy/ehy.c \
	primitives/ell/ell.c \
	primitives/epa/epa.c \
	primitives/eto/eto.c \
	primitives/extrude/extrude.c \
	primitives/grip/grip.c \
	primitives/half/half.c \
	primitives/hf/hf.c \
	primitives/hyp/hyp.c \
	primitives/metaball/metaball.c \
	primitives/nmg/nmg.c \
	primitives/nmg/nmg_bool.c \
	primitives/nmg/nmg_ck.c \
	primitives/nmg/nmg_class.c \
	primitives/nmg/nmg_eval.c \
	primitives/nmg/nmg_extrude.c \
	primitives/nmg/nmg_fcut.c \
	primitives/nmg/nmg_fuse.c \
	primitives/nmg/nmg_index.c \
	primitives/nmg/nmg_info.c \
	primitives/nmg/nmg_inter.c \
	primitives/nmg/nmg_manif.c \
	primitives/nmg/nmg_mesh.c \
	primitives/nmg/nmg_misc.c \
	primitives/nmg/nmg_mk.c \
	primitives/nmg/nmg_mod.c \
	primitives/nmg/nmg_plot.c \
	primitives/nmg/nmg_pr.c \
	primitives/nmg/nmg_pt_fu.c \
	primitives/nmg/nmg_rt_isect.c \
	primitives/nmg/nmg_rt_segs.c \
	primitives/nmg/nmg_tri.c \
	primitives/nmg/nmg_visit.c \
	primitives/part/part.c \
	primitives/pipe/pipe.c \
	primitives/poly/poly.c \
	primitives/rec/rec.c \
	primitives/rhc/rhc.c \
	primitives/rpc/rpc.c \
	primitives/sketch/sketch.c \
	primitives/sph/sph.c \
	primitives/submodel/submodel.c \
	primitives/superell/superell.c \
	primitives/tor/tor.c \
	primitives/tgc/tgc.c \
	primitives/vol/vol.c \
	regionfix.c \
	roots.c \
	shoot.c \
	spectrum.c \
	storage.c \
	table.c \
	tcl.c \
	timer42.c \
	transform.c \
	tree.c \
	vers.c \
	vlist.c \
	wdb.c

librt_nil_la_LIBADD = ${BREP_LIBADD}

librt_la_SOURCES =
librt_la_LDFLAGS = -version-info 19:1 -no-undefined -Wc,${FEXCEPTIONS}
librt_la_LIBADD = \
	librt_nil.la \
	${RT_LIBS}

comb_SOURCES = comb.c
comb_LDADD = \
	librt.la \
	${RT_LIBS}

nurb_example_SOURCES = nurb_example.c
nurb_example_LDADD = \
	librt.la \
	${RT_LIBS}

noinst_HEADERS = \
	primitives/fixpt.h \
	primitives/xxx/xxx.h

dist_man_MANS = librt.3

sample_applicationsdir = $(SAMPLE_APPLICATIONS_DIR)

sample_applications_DATA = \
	nurb_example.c \
	raydebug.tcl

# XXX need to fix these so the timers are selected by configure.ac
EXTRA_DIST = \
	$(sample_applications_DATA) \
        CMakeLists.txt \
	brep_test.cpp \
	parse.c \
	pmalloc.c \
	primitives/bot/g_bot_include.c \
	primitives/nmg/nmg_junk.c \
	timer-nt.c \
	timer52brl.c \
	timerhep.c \
	timerunix.c \
	vshoot.c

if BUILD_REGEX
DEPADD = src/other/libregex
endif

DEPENDS = src/libbn src/libsysv src/other/openNURBS ${DEPADD}

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(librt_nil_la_OBJECTS) \
	$(librt_la_OBJECTS) \
	$(lib_LTLIBRARIES) \
	$(comb_OBJECTS) \
	$(nurb_example_OBJECTS) \
	$(noinst_PROGRAMS)
