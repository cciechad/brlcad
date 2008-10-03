/*                           C M D . C
 * BRL-CAD
 *
 * Copyright (c) 1998-2008 United States Government as represented by
 * the U.S. Army Research Laboratory.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; see the file named COPYING for more
 * information.
 */
/** @addtogroup butcl */
/** @{ */
/** @file libbu/cmd.c
 *
 * @brief
 *	Utility routines for handling commands.
 *
 */

#include "common.h"

#include <string.h>
#include "bio.h"

#include "tcl.h"
#include "cmd.h"			/* includes bu.h */

/**
 *
 *	bu_cmd
 *
 *	This function is intended to be used for parsing subcommands.
 *	If the command is found in the array of commands, the associated
 *	function is called. Otherwise, an error message is created and
 *	added to interp.
 *
 *	@param clientData	- data/state associated with the command
 *	@param interp		- tcl interpreter wherein this command is registered
 *			  (Note - the result of the command is also stored here)
 *	@param argc		- number of arguments in argv
 *	@param argv		- command to execute and its arguments
 *	@param cmds		- commands and related function pointers
 *	@param cmd_index	- indicates which argv element holds the subcommand
 *
 *	@return TCL_OK if successful, otherwise, TCL_ERROR.
 */
int
bu_cmd(ClientData	clientData,
       Tcl_Interp	*interp,
       int		argc,
       char		**argv,
       struct bu_cmdtab	*cmds,
       int		cmd_index)
{
    register struct bu_cmdtab *ctp;

    /* sanity */
    if (cmd_index >= argc) {
	Tcl_AppendResult(interp,
			 "missing command; must be one of:",
			 (char *)NULL);
	goto missing_cmd;
    }

    for (ctp = cmds; ctp->ct_name != (char *)NULL; ctp++) {
	if (ctp->ct_name[0] == argv[cmd_index][0] &&
	    strcmp(ctp->ct_name, argv[cmd_index]) == 0) {
	    return (*ctp->ct_func)(clientData, interp, argc, argv);
	}
    }

    Tcl_AppendResult(interp,
		     "unknown command: ",
		     argv[cmd_index], ";",
		     " must be one of: ",
		     (char *)NULL);

 missing_cmd:
    for (ctp = cmds; ctp->ct_name != (char *)NULL; ctp++) {
	Tcl_AppendResult(interp, " ", ctp->ct_name, (char *)NULL);
    }
    Tcl_AppendResult(interp, "\n", (char *)NULL);

    return TCL_ERROR;
}

/**
 *
 *	bu_register_cmds
 *
 *	This is a convenience routine for registering an array of commands
 *	with a Tcl interpreter. Note - this is not intended for use by
 *	commands with associated state (i.e. ClientData).
 *
 *	@param interp		- Tcl interpreter wherein to register the commands
 *	@param cmds		- commands and related function pointers
 *
 * @return
 *	void
 */
void
bu_register_cmds(Tcl_Interp		*interp,
		 struct bu_cmdtab	*cmds)
{
    register struct bu_cmdtab *ctp;

    for (ctp = cmds; ctp->ct_name != (char *)NULL; ctp++)
	(void)Tcl_CreateCommand(interp, ctp->ct_name, ctp->ct_func,
				(ClientData)ctp, (Tcl_CmdDeleteProc *)NULL);
}
/** @} */
/*
 * Local Variables:
 * mode: C
 * tab-width: 8
 * indent-tabs-mode: t
 * c-file-style: "stroustrup"
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
