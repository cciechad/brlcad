# $Id: Makefile.am 32183 2008-08-01 04:24:50Z starseeker $

SUBDIRS = \
	demos \
	src \
	library \
	win

DIST_SUBDIRS = \
	$(SUBDIRS) \
	cf \
	examples \
	html \
	man

AM_CPPFLAGS = \
	-I${top_srcdir}/src/other/blt/src \
	-DTCL_LIBRARY="\"${TCL_LIBRARY}\"" \
	-DTCL_PACKAGE_PATH="\"${TCL_PACKAGE_PATH}\""

AM_CFLAGS = ${X_CFLAGS}

lib_LTLIBRARIES = \
	libBLT.la \
	libBLT@BLT_SUFFIX@.la

libBLT_la_SOURCES =
libBLT_la_LIBADD = \
	src/bltAlloc.lo \
	src/bltArrayObj.lo \
	src/bltBeep.lo \
	src/bltBgexec.lo \
	src/bltBind.lo \
	src/bltBitmap.lo \
	src/bltBusy.lo \
	src/bltCanvEps.lo \
	src/bltChain.lo \
	src/bltColor.lo \
	src/bltConfig.lo \
	src/bltContainer.lo \
	src/bltCutbuffer.lo \
	src/bltDebug.lo \
	src/bltDragdrop.lo \
	src/bltGraph.lo \
	src/bltGrAxis.lo \
	src/bltGrBar.lo \
	src/bltGrElem.lo \
	src/bltGrGrid.lo \
	src/bltGrHairs.lo \
	src/bltGrLegd.lo \
	src/bltGrLine.lo \
	src/bltGrMarker.lo \
	src/bltGrMisc.lo \
	src/bltGrPen.lo \
	src/bltGrPs.lo \
	src/bltHash.lo \
	src/bltHierbox.lo \
	src/bltHtext.lo \
	src/bltImage.lo \
	src/bltInit.lo \
	src/bltList.lo \
	src/bltNsUtil.lo \
	src/bltObjConfig.lo \
	src/bltParse.lo \
	src/bltPool.lo \
	src/bltPs.lo \
	src/bltScrollbar.lo \
	src/bltSpline.lo \
	src/bltSwitch.lo \
	src/bltTable.lo \
	src/bltTabnotebook.lo \
	src/bltTabset.lo \
	src/bltTed.lo \
	src/bltText.lo \
	src/bltTile.lo \
	src/bltTree.lo \
	src/bltTreeCmd.lo \
	src/bltTreeView.lo \
	src/bltTreeViewCmd.lo \
	src/bltTreeViewColumn.lo \
	src/bltTreeViewEdit.lo \
	src/bltTreeViewStyle.lo \
	src/bltUnixDnd.lo \
	src/bltUnixImage.lo \
	src/bltUnixPipe.lo \
	src/bltUtil.lo \
	src/bltVecMath.lo \
	src/bltVecObjCmd.lo \
	src/bltVector.lo \
	src/bltWatch.lo \
	src/bltWindow.lo \
	src/bltWinop.lo \
	src/tkButton.lo \
	src/tkFrame.lo \
	${LIBSOCKET} \
	${LIBM} \
	${LIBNSL} \
	${TCL} \
	${TK} \
	${X_LIBS}

EXTRA_DIST = \
	MANIFEST \
	Makefile.vc \
	PROBLEMS \
	blt.mak

libBLT@BLT_SUFFIX@_la_SOURCES = $(libBLT_la_SOURCES)
libBLT@BLT_SUFFIX@_la_LDFLAGS = $(libBLT_la_LDFLAGS)
libBLT@BLT_SUFFIX@_la_LIBADD = $(libBLT_la_LIBADD)

if BUILD_TK
TKDEP = src/other/tk
else
TKDEP =
endif

DEPENDS = $(TKDEP)

include $(top_srcdir)/misc/Makefile.defs
