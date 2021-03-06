/*                          M A I N . C
 * BRL-CAD
 *
 * Copyright (c) 1998-2008 United States Government as represented by
 * the U.S. Army Research Laboratory.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; see the file named COPYING for more
 * information.
 *
 */
/** @file main.c
 *
 *  This file provides the main() function for both BWISH and BTCLSH.
 *  While initializing Tcl, Itcl and various BRL-CAD libraries it sets
 *  things up to provide command history and command line editing.
 *
 */

#include "common.h"

#include "tcl.h"

#ifdef BWISH
#  include "itk.h"
#else
#  include "itcl.h"
#endif

#include "bu.h"
#include "vmath.h"
#include "bn.h"
#include "ged.h"
#ifdef BWISH
#  include "dm.h"
#endif
#include "tclcad.h"


/* XXX -- it's probably a bad idea to import itcl/itk/iwidgets into
 * the global namespace..  allow for easy means to disable the import.
 */
#define IMPORT_ITCL	1
#define IMPORT_ITK	1
#define IMPORT_IWIDGETS	1

extern int cmdInit(Tcl_Interp *interp);
extern void Cad_Main(int argc, char **argv, Tcl_AppInitProc (*appInitProc), Tcl_Interp *interp);

#ifdef BWISH
Tk_Window tkwin;
#endif

Tcl_Interp *interp;


static int
Cad_AppInit(Tcl_Interp *interp)
{
    int try_auto_path = 0;

    int init_tcl = 1;
    int init_tk = 1;
    int init_itcl = 1;
    int init_itk = 1;
    int init_blt = 1;

    /* a two-pass init loop.  the first pass just tries default init
     * routines while the second calls tclcad_auto_path() to help it
     * find other, potentially uninstalled, resources.
     */
    while (1) {

	/* not called first time through, give Tcl_Init() a chance */
	if (try_auto_path) {
	    /* Locate the BRL-CAD-specific Tcl scripts, set the auto_path */
	    tclcad_auto_path(interp);
	}

	/* Initialize Tcl */
	Tcl_ResetResult(interp);
	if (init_tcl && Tcl_Init(interp) == TCL_ERROR) {
	    if (!try_auto_path) {
		try_auto_path=1;
		continue;
	    }
	    bu_log("Tcl_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	    return TCL_ERROR;
	}
	init_tcl=0;

	/* warn if tcl_library isn't set by now */
	if (try_auto_path) {
	    tclcad_tcl_library(interp);
	}

#ifdef BWISH
	/* Initialize Tk */
	Tcl_ResetResult(interp);
	if (init_tk && Tk_Init(interp) == TCL_ERROR) {
	    if (!try_auto_path) {
		try_auto_path=1;
		continue;
	    }
	    bu_log("Tk_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	    return TCL_ERROR;
	}
	Tcl_StaticPackage(interp, "Tk", Tk_Init, Tk_SafeInit);
	init_tk=0;
#endif

	/* Initialize [incr Tcl] */
	Tcl_ResetResult(interp);
	if (init_itcl && Itcl_Init(interp) == TCL_ERROR) {
	    if (!try_auto_path) {
		try_auto_path=1;
		continue;
	    }
	    bu_log("Itcl_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	    return TCL_ERROR;
	}
	Tcl_StaticPackage(interp, "Itcl", Itcl_Init, Itcl_SafeInit);
	init_itcl=0;

#ifdef BWISH
	/* Initialize [incr Tk] */
	Tcl_ResetResult(interp);
	if (init_itk && Itk_Init(interp) == TCL_ERROR) {
	    if (!try_auto_path) {
		try_auto_path=1;
		continue;
	    }
	    bu_log("Itk_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	    return TCL_ERROR;
	}
	Tcl_StaticPackage(interp, "Itk", Itk_Init, (Tcl_PackageInitProc *) NULL);
	init_itk=0;

	/* Initialize BLT */
	Tcl_ResetResult(interp);
	if (init_blt && Blt_Init(interp) == TCL_ERROR) {
	    if (!try_auto_path) {
		try_auto_path=1;
		continue;
	    }
	    bu_log("Blt_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	    return TCL_ERROR;
	}
	Tcl_StaticPackage(interp, "BLT", Blt_Init, (Tcl_PackageInitProc *) NULL);
	init_blt=0;
#endif

	/* don't actually want to loop forever */
	break;

    } /* end iteration over Init() routines that need auto_path */
    Tcl_ResetResult(interp);

    /* if we haven't loaded by now, load auto_path so we find our tclscripts */
    if (!try_auto_path) {
	/* Locate the BRL-CAD-specific Tcl scripts */
	tclcad_auto_path(interp);
    }

#ifdef IMPORT_ITCL
    /* Import [incr Tcl] commands into the global namespace. */
    if (Tcl_Import(interp, Tcl_GetGlobalNamespace(interp),
		   "::itcl::*", /* allowOverwrite */ 1) != TCL_OK) {
	bu_log("Tcl_Import ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }
#endif /* IMPORT_ITCL */

#ifdef BWISH

#  ifdef IMPORT_ITK
    /* Import [incr Tk] commands into the global namespace */
    if (Tcl_Import(interp, Tcl_GetGlobalNamespace(interp),
		   "::itk::*", /* allowOverwrite */ 1) != TCL_OK) {
	bu_log("Tcl_Import ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }
#  endif  /* IMPORT_ITK */

    /* Initialize the Iwidgets package */
    if (Tcl_Eval(interp, "package require Iwidgets") != TCL_OK) {
	bu_log("Tcl_Eval ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }

#  ifdef IMPORT_IWIDGETS
    /* Import iwidgets into the global namespace */
    if (Tcl_Import(interp, Tcl_GetGlobalNamespace(interp),
		   "::iwidgets::*", /* allowOverwrite */ 1) != TCL_OK) {
	bu_log("Tcl_Import ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }
#  endif  /* IMPORT_IWIDGETS */

#endif  /* BWISH */

#  ifdef IMPORT_ITCL
    if (Tcl_Eval(interp, "auto_mkindex_parser::slavehook { _%@namespace import -force ::itcl::* }") != TCL_OK) {
	bu_log("Tcl_Eval ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }
#  endif

#ifdef BWISH
#  ifdef IMPORT_ITCL
    if (Tcl_Eval(interp, "auto_mkindex_parser::slavehook { _%@namespace import -force ::tk::* }") != TCL_OK) {
	bu_log("Tcl_Eval ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }
#  endif
#  ifdef IMPORT_ITK
    if (Tcl_Eval(interp, "auto_mkindex_parser::slavehook { _%@namespace import -force ::itk::* }") != TCL_OK) {
	bu_log("Tcl_Eval ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }
#  endif

    /* Initialize libdm */
    if (Dm_Init(interp) == TCL_ERROR) {
	bu_log("Dm_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Dm", Dm_Init, (Tcl_PackageInitProc *) NULL);

    /* Initialize libfb */
    if (Fb_Init(interp) == TCL_ERROR) {
	bu_log("Fb_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Fb", Fb_Init, (Tcl_PackageInitProc *) NULL);

#endif

    /* Initialize libbu */
    if (Bu_Init(interp) == TCL_ERROR) {
	bu_log("Bu_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }

    /* Initialize libbn */
    if (Bn_Init(interp) == TCL_ERROR) {
	bu_log("Bn_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }

    /* Initialize librt */
    if (Rt_Init(interp) == TCL_ERROR) {
	bu_log("Rt_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Rt", Rt_Init, (Tcl_PackageInitProc *) NULL);

    /* Initialize libged */
    if (Ged_Init(interp) == TCL_ERROR) {
	bu_log("Ged_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }

#ifdef BWISH
    if ((tkwin = Tk_MainWindow(interp)) == NULL)
	return TCL_ERROR;
#endif

    /* register bwish/btclsh commands */
    cmdInit(interp);

    return TCL_OK;
}

int
main(int argc, char **argv)
{
    /* Create the interpreter */
    interp = Tcl_CreateInterp();
    Cad_Main(argc, argv, Cad_AppInit, interp);

    return 0;
}


/*
 * Local Variables:
 * mode: C
 * tab-width: 8
 * indent-tabs-mode: t
 * c-file-style: "stroustrup"
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
