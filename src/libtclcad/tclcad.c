/*                        T C L C A D . C
 * BRL-CAD
 *
 * Copyright (c) 2004-2008 United States Government as represented by
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
/** @file tclcad.c
 *
 * Initialize BRL-CAD's Tcl interface.
 *
 */

#include "common.h"

#include "tclcad.h"

#define RESOURCE_INCLUDED 1
#include <tcl.h>
#include <itcl.h>
#include <itk.h>
#include <blt.h>
#include "bio.h"

#include "bu.h"
#include "dm.h"
#include "fb.h"
#include "bn.h"
#include "vmath.h"
#include "raytrace.h"

/* Private headers */
#include "brlcad_version.h"

int
Tclcad_Init(Tcl_Interp *interp)
{
#if 0
    if (Tcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }

    if (Tk_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
#endif

    /* Locate the BRL-CAD-specific Tcl scripts, set the auto_path */
    tclcad_auto_path(interp);

    /* Initialize [incr Tcl] */
    if (Itcl_Init(interp) == TCL_ERROR) {
	bu_log("Itcl_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }

    /* Initialize [incr Tk] */
    if (Itk_Init(interp) == TCL_ERROR) {
	bu_log("Itk_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }

    /* Initialize BLT */
    if (Blt_Init(interp) == TCL_ERROR) {
	bu_log("Blt_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }

    /* Initialize the Iwidgets package */
    if (Tcl_Eval(interp, "package require Iwidgets") != TCL_OK) {
	bu_log("Tcl_Eval ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }

    /* Initialize libdm */
    if (Dm_Init(interp) == TCL_ERROR) {
	bu_log("Dm_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }

    /* Initialize libfb */
    if (Fb_Init(interp) == TCL_ERROR) {
	bu_log("Fb_Init ERROR:\n%s\n", Tcl_GetStringResult(interp));
	return TCL_ERROR;
    }

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

    Tcl_PkgProvide(interp,  "Tclcad", (ClientData)brlcad_version());

    return TCL_OK;
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
