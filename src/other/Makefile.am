# $Id: Makefile.am 30535 2008-03-17 14:33:36Z brlcad $

if BUILD_REGEX
REGEXDIR=libregex
else !BUILD_REGEX
REGEXDIR=
endif

if BUILD_ZLIB
ZDIR=libz
else !BUILD_ZLIB
ZDIR=
endif

if BUILD_PNG
PNGDIR=libpng
else !BUILD_PNG
PNGDIR=
endif

if BUILD_JOVE
JOVEDIR=jove
else !BUILD_JOVE
JOVEDIR=
endif

if BUILD_URT
URTDIR=URToolkit
UTAHRLEDIR=libutahrle
else !BUILD_URT
URTDIR=
UTAHRLEDIR=
endif

if BUILD_TCL
TCLDIR=tcl
else !BUILD_TCL
TCLDIR=
endif

if BUILD_TK
if WITH_X11
TKDIR=tk
else !WITH_X11
TKDIR=
endif
else !BUILD_TK
TKDIR=
endif

if BUILD_ITCL
ITCLDIR=incrTcl
else !BUILD_ITCL
ITCLDIR=
endif

if BUILD_IWIDGETS
IWIDGETSDIR=iwidgets
else !BUILD_IWIDGETS
IWIDGETSDIR=
endif

if BUILD_TERMLIB
TERMLIBDIR=libtermlib
else !BUILD_TERMLIB
TERMLIBDIR=
endif

if BUILD_TKIMG
if WITH_X11
TKIMGDIR=tkimg
else !WITH_X11
TKIMGDIR=
endif
else !BUILD_TKIMG
TKIMGDIR=
endif

if BUILD_BLT
if WITH_X11
BLTDIR=blt
else !WITH_X11
BLTDIR=
endif
else !BUILD_BLT
BLTDIR=
endif

if BUILD_OPENNURBS
OPENNURBSDIR=openNURBS
else !BUILD_OPENNURBS
OPENNURBSDIR=
endif

if BUILD_TNT
TNTDIR=tnt
else !BUILD_TNT
TNTDIR=
endif


if ONLY_BENCHMARK
SUBDIRS = \
	${ZDIR} \
	${PNGDIR} \
	${REGEXDIR} \
	${TCLDIR} \
	${OPENNURBSDIR} \
	${TNTDIR}

else !ONLY_BENCHMARK

if ONLY_RTS
SUBDIRS = \
	${ZDIR} \
	${PNGDIR} \
	${REGEXDIR} \
	${TCLDIR} \
	${OPENNURBSDIR} \
	${TNTDIR}

else !ONLY_RTS
# do not sort SUBDIRS
SUBDIRS = \
	${ZDIR} \
	${PNGDIR} \
	${REGEXDIR} \
	$(UTAHRLEDIR) \
	$(URTDIR) \
	${TCLDIR} \
	${TKDIR} \
	${ITCLDIR} \
	${IWIDGETSDIR} \
	${TERMLIBDIR} \
	${JOVEDIR} \
	${TKIMGDIR} \
	${BLTDIR} \
	${OPENNURBSDIR} \
	${TNTDIR} \
	awf \
	intaval

endif
# end ONLY_RTS

endif
# end ONLY_BENCHMARK

DIST_SUBDIRS = \
	URToolkit \
	awf \
	blt \
	incrTcl \
	intaval \
	iwidgets \
	jove \
	libpng \
	libregex \
	libtermlib \
	libutahrle \
	libz \
	openNURBS \
	tcl \
	tk \
	tkimg \
	tnt

include $(top_srcdir)/misc/Makefile.defs
