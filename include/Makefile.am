# $Id: Makefile.am 32046 2008-07-29 12:51:07Z starseeker $

SUBDIRS = conf

brlcadincludedir = $(includedir)/brlcad

brlcadinclude_required = \
	RtServerImpl.h \
	bn.h \
	brlcad_config.h \
	bu.h \
	common.h \
	config_win.h \
	ged.h \
	magic.h \
	raytrace.h \
	rtgeom.h \
	rtprivate.h \
	rtserver.h \
	tclcad.h

brlcadinclude_wanted = \
	anim.h \
	bot.h \
	brep.h \
	brlcad.h \
	cmd.h \
	db.h \
	db5.h \
	dg.h \
	dm-Null.h \
	dm-X.h \
	dm-glx.h \
	dm-ogl.h \
	dm-wgl.h \
	dm-pex.h \
	dm-plot.h \
	dm-ps.h \
	dm-tk.h \
	dm.h \
	dm_color.h \
	dm_xvars.h \
	fb.h \
	fbio.h \
	fbserv_obj.h \
	libtermio.h \
	light.h \
	mater.h \
	nmg.h \
	nurb.h \
	opennurbs_ext.h \
	optical.h \
	orle.h \
	photonmap.h \
	pkg.h \
	plastic.h \
	plot3.h \
	redblack.h \
	shadefuncs.h \
	shadework.h \
	solid.h \
	spectrum.h \
	spm.h \
	vectfont.h \
	vector_fpu.h \
	vector.h \
	vector_x86.h \
	vfont-if.h \
	vmath.h \
	wdb.h

# headers used by multiple packages but still considered private.
# public headers should NOT include these headers.
noinst_HEADERS = \
	brlcad_version.h \
	bio.h \
	conf.h \
	fbmsg.h \
	machine.h \
	sysv.h

EXTRA_DIST = \
	on_nurb.h \
	sed4

if ONLY_BENCHMARK
brlcadinclude_HEADERS = $(brlcadinclude_required)

else
# else !ONLY_BENCHMARK

if ONLY_RTS
brlcadinclude_HEADERS = $(brlcadinclude_required)

else
# else !ONLY_RTS
brlcadinclude_HEADERS = \
	$(brlcadinclude_required) \
	$(brlcadinclude_wanted)

endif
# end ONLY_RTS

endif
# end ONLY_BENCHMARK


include $(top_srcdir)/misc/Makefile.defs
