# $Id: Makefile.am 32013 2008-07-29 00:50:26Z starseeker $

lib_LTLIBRARIES = libfb.la

if WITH_WGL
IF_WGL_CFLAGS = -DIF_WGL
else
IF_WGL_CFLAGS =
endif

if WITH_X11
IF_X_CFLAGS = -DIF_X ${X_CFLAGS}
IF_X_LIBADD = ${X_LIBS}
else
IF_X_CFLAGS =
IF_X_LIBADD =
endif

if WITH_OPENGL
IF_OGL_CFLAGS = -DIF_OGL
IF_OGL_LIBADD = ${LIBGL}
else
IF_OGL_CFLAGS =
IF_OGL_LIBADD =
endif

if WITH_TK
IF_TK_CFLAGS = -DIF_TK $(TK_CPPFLAGS)
IF_TK_LIBADD = ${LIBTK}
else
IF_TK_CFLAGS =
IF_TK_LIBADD =
endif

libfb_la_SOURCES = \
	asize.c \
	fb_generic.c \
	fb_log.c \
	fb_obj.c \
	fb_paged_io.c \
	fb_rect.c \
	fb_util.c \
	fbserv_obj.c \
	getput.c \
	if_X.c \
	if_X24.c \
	if_ab.c \
	if_debug.c \
	if_disk.c \
	if_mem.c \
	if_null.c \
	if_ogl.c \
	if_remote.c \
	if_stack.c \
	if_tk.c \
	if_wgl.c \
	tcl.c \
	vers.c

libfb_la_LDFLAGS = -version-info 19:1 -no-undefined
libfb_la_CFLAGS = \
	-DIF_REMOTE \
	-DIF_AB \
	${GL_CFLAGS} \
	$(IF_X_CFLAGS) \
	$(IF_TK_CFLAGS) \
	$(IF_OGL_CFLAGS) \
	$(IF_WGL_CFLAGS) \
	${TCL_CPPFLAGS}

libfb_la_LIBADD = \
	${FB_LIBS} \
	${IF_X_LIBADD} \
	${IF_OGL_LIBADD} \
	${IF_TK_LIBADD}

noinst_HEADERS = \
	adage.h \
	adagecursor.h \
	adageframe.h \
	dmdfb.h \
	nilcursor.h \
	oglcursor.h \
	sgicursor.h

dist_man_MANS = libfb.3

# XXX lots of work is needed here to make the other if_ interfaces available.
EXTRA_DIST = \
	if_4d.c \
	if_TEMPLATE.c \
	if_adage.c \
	if_ap.c \
	if_ptty.c \
	if_rat.c \
	if_sgi.c \
	if_sgiw.c \
	if_sun.c \
	if_ts.c \
	if_ug.c \
	libfb.dsp \
	sgiselect.s


DEPS=src/libpkg

include $(top_srcdir)/misc/Makefile.defs
