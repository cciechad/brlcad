# $Id: Makefile.am 32013 2008-07-29 00:50:26Z starseeker $

AM_CPPFLAGS = ${TCL_CPPFLAGS}

if WITH_OPENGL
OGL_SRCS = dm-ogl.c
OGL_CFLAGS = -DDM_OGL -DIF_OGL $(GL_CFLAGS)
else
OGL_SRCS =
OGL_CFLAGS =
endif


if WITH_WGL
WGL_SRCS = dm-wgl.c
WGL_CFLAGS = -DDM_WGL -DIF_WGL
else
WGL_SRCS =
WGL_CFLAGS =
endif

if WITH_X11
X11_SRCS = dm-X.c
X11_CFLAGS = -DDM_X -DIF_X
else
X11_SRCS =
X11_CFLAGS =
endif

if WITH_TK
TK_SRCS = dm-tk.c
TK_CFLAGS = -DDM_TK -DIF_TK
else
TK_SRCS =
TK_CFLAGS =
endif

points_dir = points
if WITH_PARSERS
POINTS = ${points_dir}
POINTS_CFLAGS = -DENABLE_POINTS
POINTS_LDADD = ${LIBL} $(top_builddir)/src/mged/points/libpoints.la
else
POINTS_DIR =
POINTS_CFLAGS =
POINTS_LDADD =
endif

SUBDIRS = $(POINTS)

DIST_SUBDIRS = ${points_dir}

bin_PROGRAMS = \
	cad_boundp \
	cad_parea \
	mged

mged_SOURCES = \
	adc.c \
	anal.c \
	animedit.c \
	arbs.c \
	attach.c \
	axes.c \
	bodyio.c \
	buttons.c \
	chgmodel.c \
	chgtree.c \
	chgview.c \
	cmd.c \
	clone.c \
	color_scheme.c \
	columns.c \
	concat.c \
	dir.c \
	$(OGL_SRCS) \
	$(WGL_SRCS) \
	$(X11_SRCS) \
	$(TK_SRCS) \
	dm-generic.c \
	dm-plot.c \
	dm-ps.c \
	dodraw.c \
	doevent.c \
	dozoom.c \
	edarb.c \
	edars.c \
	edpipe.c \
	edsol.c \
	facedef.c \
	fbserv.c \
	grid.c \
	history.c \
	inside.c \
	mater.c \
	menu.c \
	mged.c \
	mover.c \
	muves.c \
	overlay.c \
	plot.c \
	polyif.c \
	predictor.c \
	qray.c \
	rect.c \
	red.c \
	rtif.c \
	scroll.c \
	set.c \
	setup.c \
	share.c \
	solids_on_ray.c \
	tedit.c \
	titles.c \
	track.c \
	typein.c \
	update.c \
	usepen.c \
	utility1.c \
	utility2.c \
	vdraw.c \
	vparse.c \
	vrlink.c

# per-program CPPFLAGS wasn't added until automake 1.7
mged_CFLAGS = \
	$(OGL_CFLAGS) \
	$(WGL_CFLAGS) \
	$(X11_CFLAGS) \
	$(TK_CFLAGS) \
	$(POINTS_CFLAGS) \
	${PNG_CPPFLAGS}	\
	${ITCL_CPPFLAGS} \
	${ITK_CPPFLAGS} \
	${TK_CPPFLAGS} \
	-I$(top_srcdir)/src/other/tcl/generic \
	-I$(top_srcdir)/src/other/tcl/unix \
	-I$(top_srcdir)/src/other/tk/generic \
	-I$(top_srcdir)/src/other/tk/unix

mged_LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) -o $@
mged_LDADD = \
	${POINTS_LDADD} \
	${TERMIO} \
	${WDB} \
	${GED} \
	${DM} ${DM_LIBS} \
	${ITK} \
	${ITCL} \
	${TK} \
	${TCLCAD}

EXTRA_PROGRAMS = sged
sged_SOURCES = $(mged_SOURCES)
sged_LDFLAGS = -static
sged_CFLAGS = $(mged_CFLAGS)
sged_LDADD = $(mged_LDADD)

cad_boundp_SOURCES = cad_boundp.c
cad_boundp_LDADD = ${BU}

cad_parea_SOURCES = cad_parea.c
cad_parea_LDADD = ${BU}

noinst_HEADERS = \
	cmd.h \
	comb_bool.h \
	editobjGui.h \
	fbserv.h \
	htmlLibraryUi.h \
	icreateGui.h \
	joints.h \
	menu.h \
	mgdev.h \
	mged.h \
	mged_dm.h \
	mged_solid.h \
	mgedtcl.h \
	objdir.h \
	qray.h \
	scroll.h \
	sedit.h \
	sunicon.h \
	titles.h \
	vgdev.h \
	vld_std.h \
	vmathUi.h

man_MANS = \
	cad_boundp.1 \
	cad_parea.1 \
	mged.1

INCLUDES = \
	${X_CFLAGS} \
	-I$(top_srcdir)/include

EXTRA_DIST = \
	$(man_MANS) \
	arb.c \
	bool_rewrite.c \
	comb_bool_parse.y \
	comb_bool_scan.l \
	dm-ogl.c \
	dm-wgl.c \
	dm-X.c \
	dm-tk.c \
	fbserv_win32.c \
	hideline.c \
	memalloc.c \
	mged.bat \
	mged.dsp

DEPS=src/libtermio src/libwdb src/libdm src/other/incrTcl src/libtclcad

include $(top_srcdir)/misc/Makefile.defs

FAST_OBJECTS = \
	$(cad_boundp_OBJECTS) \
	$(cad_parea_OBJECTS) \
	$(mged_OBJECTS) \
	$(bin_PROGRAMS)

