# $Id: Makefile.am 29763 2007-12-17 19:17:17Z brlcad $

AM_CPPFLAGS = \
	-DNO_MOUNTAIN \
	-DTCL_LIBRARY="\"${TCL_LIBRARY}\"" \
	-DTCL_PACKAGE_PATH="\"${TCL_PACKAGE_PATH}\"" \
	-I${top_srcdir}/src/other/blt/src \
	${TCL_CPPFLAGS} \
	${TK_CPPFLAGS}

AM_CFLAGS = ${X_CFLAGS}

noinst_LTLIBRARIES = libblts.la

libblts_la_SOURCES = \
	bltAlloc.c \
	bltArrayObj.c \
	bltBeep.c \
	bltBgexec.c \
	bltBind.c \
	bltBitmap.c \
	bltBusy.c \
	bltCanvEps.c \
	bltChain.c \
	bltColor.c \
	bltConfig.c \
	bltContainer.c \
	bltCutbuffer.c \
	bltDebug.c \
	bltDragdrop.c \
	bltGraph.c \
	bltGrAxis.c \
	bltGrBar.c \
	bltGrElem.c \
	bltGrGrid.c \
	bltGrHairs.c \
	bltGrLegd.c \
	bltGrLine.c \
	bltGrMarker.c \
	bltGrMisc.c \
	bltGrPen.c \
	bltGrPs.c \
	bltHash.c \
	bltHierbox.c \
	bltHtext.c \
	bltImage.c \
	bltInit.c \
	bltList.c \
	bltNsUtil.c \
	bltObjConfig.c \
	bltParse.c \
	bltPool.c \
	bltPs.c \
	bltScrollbar.c \
	bltSpline.c \
	bltSwitch.c \
	bltTable.c \
	bltTabnotebook.c \
	bltTabset.c \
	bltTed.c \
	bltText.c \
	bltTile.c \
	bltTree.c \
	bltTreeCmd.c \
	bltTreeView.c \
	bltTreeViewCmd.c \
	bltTreeViewColumn.c \
	bltTreeViewEdit.c \
	bltTreeViewStyle.c \
	bltUnixDnd.c \
	bltUnixImage.c \
	bltUnixPipe.c \
	bltUtil.c \
	bltVecMath.c \
	bltVecObjCmd.c \
	bltVector.c \
	bltWatch.c \
	bltWindow.c \
	bltWinop.c \
	tkButton.c \
	tkConsole.c \
	tkFrame.c

EXTRA_DIST = \
	Makefile-cyg.in \
	Makefile.bc \
	Makefile.vc \
	blt.mak \
	bltConfig.h \
	bltTuple.c \
	bltTupleCmd.c \
	bltUnixMain.c \
	bltVecCmd.c \
	bltWinConfig.h \
	bltWinDde.c \
	bltWinDraw.c \
	bltWinImage.c \
	bltWinMain.c \
	bltWinPipe.c \
	bltWinPrnt.c \
	bltWinUtil.c \
	bltWindow.c \
	bltWinop.c \
	pure_api.c \
	tkMenubutton.c \
	tkScrollbar.c

include_HEADERS = \
	blt.h \
	bltBind.h \
	bltChain.h \
	bltGraph.h \
	bltGrAxis.h \
	bltGrElem.h \
	bltGrLegd.h \
	bltHash.h \
	bltImage.h \
	bltInterp.h \
	bltInt.h \
	bltList.h \
	bltMath.h \
	bltNsUtil.h \
	bltObjConfig.h \
	bltPool.h \
	bltPs.h \
	bltSwitch.h \
	bltTable.h \
	bltText.h \
	bltTile.h \
	bltTkInt.h \
	bltTree.h \
	bltTreeView.h \
	bltTuple.h \
	bltVecInt.h \
	bltVector.h \
	bltWait.h \
	bltWin.h \
	missing.h

brlcadincludedir = $(includedir)/brlcad
brlcadinclude_HEADERS = $(include_HEADERS)

include $(top_srcdir)/misc/Makefile.defs
