# $Id: Makefile.am 32013 2008-07-29 00:50:26Z starseeker $

lib_LTLIBRARIES = liboptical.la

AM_CPPFLAGS = ${TCL_CPPFLAGS}

liboptical_la_SOURCES = \
	init.c \
	material.c \
	photonmap.c \
	refract.c \
	sh_air.c \
	sh_billboard.c \
	sh_brdf.c \
	sh_camo.c \
	sh_cloud.c \
	sh_cook.c \
	sh_fbm.c \
	sh_fire.c \
	sh_flat.c \
	sh_gauss.c \
	sh_grass.c \
	sh_light.c \
	sh_noise.c \
	sh_null.c \
	sh_plastic.c \
	sh_points.c \
	sh_prj.c \
	sh_rtrans.c \
	sh_scloud.c \
	sh_spm.c \
	sh_stack.c \
	sh_stxt.c \
	sh_tcl.c \
	sh_text.c \
	sh_toyota.c \
	sh_treetherm.c \
	sh_wood.c \
	sh_xxx.c \
	shade.c \
	turb.c \
	vers.c \
	wray.c

liboptical_la_LDFLAGS = -version-info 19:1 -no-undefined
liboptical_la_LIBADD = \
	${OPTICAL_LIBS}

# convenience library, must NOT be set to static in here
noinst_LTLIBRARIES = librtms.la

librtms_la_CFLAGS = -DRT_MULTISPECTRAL
librtms_la_SOURCES = \
	sh_camo.c \
	sh_light.c \
	sh_noise.c \
	sh_plastic.c \
	sh_stack.c \
	shade.c

EXTRA_DIST = \
	liboptical.dsp

DEPS=src/librt

include $(top_srcdir)/misc/Makefile.defs
