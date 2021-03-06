/*                        U P D A T E . C
 * BRL-CAD
 *
 * Copyright (c) 1995-2008 United States Government as represented by
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
 */
/** @file update.c
 *
 */

#include "common.h"

#include <string.h>

#ifdef HAVE_X11_XLIB_H
#  include <X11/Xlib.h>
#endif

#include "tcl.h"
#include "tk.h"

#include "bu.h"

#include "./mgedtcl.h"


extern void refresh();
extern int event_check();


void
mged_update(int	non_blocking)
{
    if (non_blocking >= 0)
	event_check(non_blocking);
    refresh();
}

int
f_update(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    int non_blocking;

    if (argc != 2 || sscanf(argv[1], "%d", &non_blocking) != 1) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "helpdevel mged_update");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    mged_update(non_blocking);

    return TCL_OK;
}


/*
 * Copied from libtk/generic/tkCmds.c. Used by
 * the f_wait() procedure.
 */
static char *
WaitVariableProc(ClientData	clientData,	/* Pointer to integer to set to 1. */
		 Tcl_Interp	*interp,	/* Interpreter containing variable. */
		 char		*name1,		/* Name of variable. */
		 char		*name2,		/* Second part of variable name. */
		 int		flags)		/* Information about what happened. */
{
    int *donePtr = (int *) clientData;

    *donePtr = 1;
    return (char *) NULL;
}


/*
 * Copied from libtk/generic/tkCmds.c. Used by
 * the f_wait() procedure.
 */
static void
WaitVisibilityProc(ClientData	clientData,	/* Pointer to integer to set to 1. */
		   XEvent	*eventPtr)	/* Information about event (not used). */
{
    int *donePtr = (int *) clientData;

    if (eventPtr->type == VisibilityNotify) {
	*donePtr = 1;
    }
    if (eventPtr->type == DestroyNotify) {
	*donePtr = 2;
    }
}

/*
 * Copied from libtk/generic/tkCmds.c. Used by
 * the f_wait() procedure.
 */
static void
WaitWindowProc(ClientData	clientData,	/* Pointer to integer to set to 1. */
	       XEvent		*eventPtr)	/* Information about event. */
{
    int *donePtr = (int *) clientData;

    if (eventPtr->type == DestroyNotify) {
	*donePtr = 1;
    }
}

/*
 * This procedure is a slightly modified version of the Tk_TkwaitCmd.
 * It was modified to use mged_update so that the geometry windows
 * would get refreshed.
 */
int
f_wait(ClientData	clientData,	/* Main window associated with interpreter. */
       Tcl_Interp	*interp,	/* Current interpreter. */
       int		argc,		/* Number of arguments. */
       char		**argv)		/* Argument strings. */
{
    int c, done;
    size_t length;
    Tk_Window window;

    if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
			 argv[0], " variable|visibility|window name\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'v') && (strncmp(argv[1], "variable", length) == 0)
	&& (length >= 2)) {
	if (Tcl_TraceVar(interp, argv[2],
			 TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
			 (Tcl_VarTraceProc *)WaitVariableProc,
			 (ClientData) &done) != TCL_OK) {
	    return TCL_ERROR;
	}
	done = 0;
	while (!done) {
	    mged_update(0);
	}
	Tcl_UntraceVar(interp, argv[2],
		       TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		       (Tcl_VarTraceProc *)WaitVariableProc,
		       (ClientData) &done);
    } else if ((c == 'v') && (strncmp(argv[1], "visibility", length) == 0)
	       && (length >= 2)) {

	window = Tk_NameToWindow(interp, argv[2], tkwin);
	if (window == NULL) {
	    return TCL_ERROR;
	}
	Tk_CreateEventHandler(window, VisibilityChangeMask|StructureNotifyMask,
			      WaitVisibilityProc, (ClientData) &done);
	done = 0;
	while (!done) {
	    mged_update(0);
	}
	if (done != 1) {
	    /*
	     * Note that we do not delete the event handler because it
	     * was deleted automatically when the window was destroyed.
	     */

	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, "window \"", argv[2],
			     "\" was deleted before its visibility changed",
			     (char *) NULL);
	    return TCL_ERROR;
	}
	Tk_DeleteEventHandler(window, VisibilityChangeMask|StructureNotifyMask,
			      WaitVisibilityProc, (ClientData) &done);
    } else if ((c == 'w') && (strncmp(argv[1], "window", length) == 0)) {
	window = Tk_NameToWindow(interp, argv[2], tkwin);
	if (window == NULL) {
	    return TCL_ERROR;
	}
	Tk_CreateEventHandler(window, StructureNotifyMask,
			      WaitWindowProc, (ClientData) &done);
	done = 0;
	while (!done) {
	    mged_update(0);
	}
	/*
	 * Note:  there's no need to delete the event handler.  It was
	 * deleted automatically when the window was destroyed.
	 */
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
			 "\": must be variable, visibility, or window", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Clear out the interpreter's result, since it may have been set
     * by event handlers.
     */

    Tcl_ResetResult(interp);
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
