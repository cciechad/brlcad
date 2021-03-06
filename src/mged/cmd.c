/*                           C M D . C
 * BRL-CAD
 *
 * Copyright (c) 1985-2008 United States Government as represented by
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
/** @file cmd.c
 *
 * The hooks to most of mged's commands when running in console mode.
 *
 */

#include "common.h"

#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif
#include "bio.h"

#include "tcl.h"
#include "tk.h"

#include "itcl.h"

#include "bu.h"
#include "vmath.h"
#include "bn.h"
#include "rtgeom.h"
#include "dg.h"
#include "tclcad.h"

#include "./mged.h"
#include "./cmd.h"
#include "./mged_solid.h"
#include "./mged_dm.h"
#include "./sedit.h"


extern void update_grids(fastf_t sf);		/* in grid.c */
extern void set_localunit_TclVar(void);		/* in chgmodel.c */
extern void init_qray(void);			/* in qray.c */
extern int gui_setup(char *dstr);		/* in attach.c */
extern int mged_default_dlist;			/* in attach.c */
extern int classic_mged;
extern int bot_vertex_fuse(), bot_condense();
struct cmd_list head_cmd_list;
struct cmd_list *curr_cmd_list;

extern int db_warn;
extern int db_upgrade;
extern int db_version;

extern struct rt_tess_tol     mged_ttol;
extern struct bn_tol	      mged_tol;

int glob_compat_mode = 1;
int output_as_return = 1;
Tk_Window tkwin = NULL;

/* The following is for GUI output hooks: contains name of function to
 * run with output.
 */
static struct bu_vls tcl_output_hook;



/**
 *                        O U T P U T _ C A T C H
 *
 * Gets the output from bu_log and appends it to clientdata vls.
 */
HIDDEN int
output_catch(genptr_t clientdata, genptr_t str)
{
    register struct bu_vls *vp = (struct bu_vls *)clientdata;
    register int len;

    BU_CK_VLS(vp);
    len = bu_vls_strlen(vp);
    bu_vls_strcat(vp, str);
    len = bu_vls_strlen(vp) - len;

    return len;
}

/**
 *                 S T A R T _ C A T C H I N G _ O U T P U T
 *
 * Sets up hooks to bu_log so that all output is caught in the given vls.
 *
 */
void
start_catching_output(struct bu_vls *vp)
{
    bu_log_add_hook(output_catch, (genptr_t)vp);
}

/**
 *                 S T O P _ C A T C H I N G _ O U T P U T
 *
 * Turns off the output catch hook.
 */
void
stop_catching_output(struct bu_vls *vp)
{
    bu_log_delete_hook(output_catch, (genptr_t)vp);
}

/**
 *                            G U I _ O U T P U T
 *
 * Used as a hook for bu_log output.  Sends output to the Tcl procedure whose
 * name is contained in the vls "tcl_output_hook".  Useful for user interface
 * building.
 */
int
gui_output(genptr_t clientData, genptr_t str)
{
    Tcl_DString tclcommand;
    Tcl_Obj *save_result;
    static int level = 0;

    if (level > 50) {
	bu_log_delete_hook(gui_output, clientData);
	/* Now safe to run bu_log? */
	bu_log("Ack! Something horrible just happened recursively.\n");
	return 0;
    }

    Tcl_DStringInit(&tclcommand);
    (void)Tcl_DStringAppendElement(&tclcommand, bu_vls_addr(&tcl_output_hook));
    (void)Tcl_DStringAppendElement(&tclcommand, str);

    save_result = Tcl_GetObjResult(interp);
    Tcl_IncrRefCount(save_result);
    ++level;
    Tcl_Eval((Tcl_Interp *)clientData, Tcl_DStringValue(&tclcommand));
    --level;
    Tcl_SetObjResult(interp, save_result);
    Tcl_DecrRefCount(save_result);

    Tcl_DStringFree(&tclcommand);
    return strlen(str);
}


/**
 * c m d _ g e d _ c m d
 *
 * command wrapper for all new libged commands
 */
int
cmd_ged_cmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    Tcl_DString ds;
    int ret;
    
    CHECK_DBI_NULL;
    CHECK_READ_ONLY;

    /* !!! */
}


/**
 *                     C M D _ T K
 *
 *  Command for initializing the Tk window and defaults.
 *
 *  Usage:  loadtk [displayname[.screennum]]
 */
int
cmd_tk(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    int status;

    if (argc < 1 || 2 < argc) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "help loadtk");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    if (argc == 1)
	status = gui_setup((char *)NULL);
    else
	status = gui_setup(argv[1]);

    return status;
}

/**
 *   C M D _ O U T P U T _ H O O K
 *
 *   Hooks the output to the given output hook.
 *   Removes the existing output hook!
 */
int
cmd_output_hook(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    struct bu_vls infocommand;
    int status;

    if (argc < 1 || 2 < argc) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "helpdevel output_hook");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    bu_log_delete_hook(gui_output, (genptr_t)interp);/* Delete the existing hook */

    if (argc < 2)
	return TCL_OK;

    /* Make sure the command exists before putting in the hook! */
    /* Note - the parameters to proc could be wrong and/or the proc could still disappear later */
    bu_vls_init(&infocommand);
    bu_vls_strcat(&infocommand, "info commands ");
    bu_vls_strcat(&infocommand, argv[1]);
    status = Tcl_Eval(interp, bu_vls_addr(&infocommand));
    bu_vls_free(&infocommand);

    if (status != TCL_OK || Tcl_GetStringResult(interp)[0] == '\0') {
	Tcl_AppendResult(interp, "command does not exist", (char *)NULL);
	return TCL_ERROR;
    }

    /* Also, don't allow silly infinite loops. */

    if (strcmp(argv[1], argv[0]) == 0) {
	Tcl_AppendResult(interp, "Don't be silly.", (char *)NULL);
	return TCL_ERROR;
    }

    /* Set up the hook! */
    bu_vls_init(&tcl_output_hook);
    bu_vls_strcpy(&tcl_output_hook, argv[1]);
    bu_log_add_hook(gui_output, (genptr_t)interp);

    Tcl_ResetResult(interp);
    return TCL_OK;
}


/**
 *			C M D _ N O P
 */
int
cmd_nop(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    return TCL_OK;
}


/**
 *			C M D _ G E T _ P T R
 *
 *  Returns an appropriately-formatted string that can later be reinterpreted
 *  (using atol() and a cast) as a a pointer.
 */
int
cmd_get_ptr(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    char buf[128];

    sprintf( buf, "%ld", (long)(*((void **)&dbip)) );
    Tcl_AppendResult( interp, buf, (char *)NULL );
    return TCL_OK;
}


int
cmd_cmd_win(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    struct bu_vls vls;

    bu_vls_init(&vls);

    if (argc < 2) {
	bu_vls_printf(&vls, "helpdevel cmd_win");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    if (strcmp(argv[1], "open") == 0) {
	struct cmd_list *clp;
	int name_not_used = 1;

	if (argc != 3) {
	    bu_vls_printf(&vls, "helpdevel cmd_win");
	    Tcl_Eval(interp, bu_vls_addr(&vls));
	    bu_vls_free(&vls);
	    return TCL_ERROR;
	}

	/* Search to see if there exists a command window with this name */
	for ( BU_LIST_FOR(clp, cmd_list, &head_cmd_list.l) )
	    if (!strcmp(argv[2], bu_vls_addr(&clp->cl_name))) {
		name_not_used = 0;
		break;
	    }

	if (name_not_used) {
	    clp = (struct cmd_list *)bu_malloc(sizeof(struct cmd_list), "cmd_list");
	    memset((void *)clp, 0, sizeof(struct cmd_list));
	    BU_LIST_APPEND(&head_cmd_list.l, &clp->l);
	    clp->cl_cur_hist = head_cmd_list.cl_cur_hist;
	    bu_vls_init(&clp->cl_more_default);
	    bu_vls_init(&clp->cl_name);
	    bu_vls_strcpy(&clp->cl_name, argv[2]);
	}

	bu_vls_free(&vls);
	return TCL_OK;
    }

    if (strcmp(argv[1], "close") == 0) {
	struct cmd_list *clp;

	if (argc != 3) {
	    bu_vls_printf(&vls, "helpdevel cmd_win");
	    Tcl_Eval(interp, bu_vls_addr(&vls));
	    bu_vls_free(&vls);
	    return TCL_ERROR;
	}

	/* First, search to see if there exists a command window with the name
	   in argv[2] */
	for ( BU_LIST_FOR(clp, cmd_list, &head_cmd_list.l) )
	    if (!strcmp(argv[2], bu_vls_addr(&clp->cl_name)))
		break;

	if (clp == &head_cmd_list) {
	    if (!strcmp(argv[2], "mged"))
		Tcl_AppendResult(interp, "cmd_close: not allowed to close \"mged\"",
				 (char *)NULL);
	    else
		Tcl_AppendResult(interp, "cmd_close: did not find \"", argv[2],
				 "\"", (char *)NULL);
	    return TCL_ERROR;
	}

	if (clp == curr_cmd_list)
	    curr_cmd_list = &head_cmd_list;

	BU_LIST_DEQUEUE( &clp->l );
	if (clp->cl_tie != NULL)
	    clp->cl_tie->dml_tie = CMD_LIST_NULL;
	bu_vls_free(&clp->cl_more_default);
	bu_vls_free(&clp->cl_name);
	bu_free((genptr_t)clp, "cmd_close: clp");

	bu_vls_free(&vls);
	return TCL_OK;
    }

    if (strcmp(argv[1], "get") == 0) {
	if (argc != 2) {
	    bu_vls_printf(&vls, "helpdevel cmd_win");
	    Tcl_Eval(interp, bu_vls_addr(&vls));
	    bu_vls_free(&vls);

	    return TCL_ERROR;
	}

	Tcl_AppendElement(interp, bu_vls_addr(&curr_cmd_list->cl_name));

	bu_vls_free(&vls);
	return TCL_OK;
    }

    if (strcmp(argv[1], "set") == 0) {
	if (argc != 3) {
	    bu_vls_printf(&vls, "helpdevel cmd_win");
	    Tcl_Eval(interp, bu_vls_addr(&vls));
	    bu_vls_free(&vls);
	    return TCL_ERROR;
	}

	for ( BU_LIST_FOR(curr_cmd_list, cmd_list, &head_cmd_list.l) ) {
	    if (strcmp(bu_vls_addr(&curr_cmd_list->cl_name), argv[2]))
		continue;

	    break;
	}

	if (curr_cmd_list->cl_tie)
	    curr_dm_list = curr_cmd_list->cl_tie;

	bu_vls_trunc(&curr_cmd_list->cl_more_default, 0);
	bu_vls_free(&vls);
	return TCL_OK;
    }

    bu_vls_printf(&vls, "helpdevel cmd_win");
    Tcl_Eval(interp, bu_vls_addr(&vls));
    bu_vls_free(&vls);

    return TCL_ERROR;
}


int
cmd_get_more_default(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    if (argc != 1) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "helpdevel get_more_default");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    Tcl_AppendResult(interp, bu_vls_addr(&curr_cmd_list->cl_more_default), (char *)NULL);
    return TCL_OK;
}

int
cmd_set_more_default(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    if (argc != 2) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "helpdevel set_more_default");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    bu_vls_strcpy(&curr_cmd_list->cl_more_default, argv[1]);
    return TCL_OK;
}


/**
 * debackslash, backslash_specials, mged_compat: routines for original
 *   mged emulation mode
 */
void
debackslash(struct bu_vls *dest, struct bu_vls *src)
{
    char *ptr;

    ptr = bu_vls_addr(src);
    while ( *ptr ) {
	if ( *ptr == '\\' )
	    ++ptr;
	if ( *ptr == '\0' )
	    break;
	bu_vls_putc( dest, *ptr++ );
    }
}


void
backslash_specials(struct bu_vls *dest, struct bu_vls *src)
{
    int backslashed;
    char *ptr, buf[2];

    buf[1] = '\0';
    backslashed = 0;
    for ( ptr = bu_vls_addr( src ); *ptr; ptr++ ) {
	if ( *ptr == '[' && !backslashed )
	    bu_vls_strcat( dest, "\\[" );
	else if ( *ptr == ']' && !backslashed )
	    bu_vls_strcat( dest, "\\]" );
	else if ( backslashed ) {
	    bu_vls_strcat( dest, "\\" );
	    buf[0] = *ptr;
	    bu_vls_strcat( dest, buf );
	    backslashed = 0;
	} else if ( *ptr == '\\' )
	    backslashed = 1;
	else {
	    buf[0] = *ptr;
	    bu_vls_strcat( dest, buf );
	}
    }
}


/**
 *                    M G E D _ C O M P A T
 *
 * This routine is called to perform wildcard expansion and character quoting
 * on the given vls (typically input from the keyboard.)
 */
void
mged_compat(struct bu_vls *dest, struct bu_vls *src, int use_first)
{
    char *start, *end;          /* Start and ends of words */
    int regexp;                 /* Set to TRUE when word is a regexp */
    int backslashed;
    int firstword;
    struct bu_vls word;         /* Current word being processed */
    struct bu_vls temp;

    if (dbip == DBI_NULL) {
	bu_vls_vlscat(dest, src);
	return;
    }

    bu_vls_init( &word );
    bu_vls_init( &temp );

    start = end = bu_vls_addr( src );
    firstword = 1;
    while ( *end != '\0' ) {
	/* Run through entire string */

	/* First, pass along leading whitespace. */

	start = end;                   /* Begin where last word ended */
	while ( *start != '\0' ) {
	    if ( *start == ' '  ||
		 *start == '\t' ||
		 *start == '\n' )
		bu_vls_putc( dest, *start++ );
	    else
		break;
	}
	if ( *start == '\0' )
	    break;

	/* Next, advance "end" pointer to the end of the word, while adding
	   each character to the "word" vls.  Also make a note of any
	   unbackslashed wildcard characters. */

	end = start;
	bu_vls_trunc( &word, 0 );
	regexp = 0;
	backslashed = 0;
	while ( *end != '\0' ) {
	    if ( *end == ' '  ||
		 *end == '\t' ||
		 *end == '\n' )
		break;
	    if ( (*end == '*' || *end == '?' || *end == '[') && !backslashed )
		regexp = 1;
	    if ( *end == '\\' && !backslashed )
		backslashed = 1;
	    else
		backslashed = 0;
	    bu_vls_putc( &word, *end++ );
	}

	if ( firstword && !use_first )
	    regexp = 0;

	/* Now, if the word was suspected of being a wildcard, try to match
	   it to the database. */

	if ( regexp ) {
	    bu_vls_trunc( &temp, 0 );
	    if ( db_regexp_match_all( &temp, dbip,
				      bu_vls_addr(&word) ) == 0 ) {
		debackslash( &temp, &word );
		backslash_specials( dest, &temp );
	    } else
		bu_vls_vlscat( dest, &temp );
	} else {
	    debackslash( dest, &word );
	}

	firstword = 0;
    }

    bu_vls_free( &temp );
    bu_vls_free( &word );
}


int
cmd_mged_glob(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    struct bu_vls dest, src;

    if (argc != 2) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "help db_glob");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    bu_vls_init(&src);
    bu_vls_init(&dest);
    bu_vls_strcpy(&src, argv[1]);
    mged_compat(&dest, &src, 0);
    Tcl_AppendResult(interp, bu_vls_addr(&dest), (char *)NULL);
    bu_vls_free(&src);
    bu_vls_free(&dest);

    return TCL_OK;
}


#ifdef _WIN32
/* limited to seconds only */
void gettimeofday(struct timeval *tp, struct timezone *tzp)
{

    time_t ltime;

    time( &ltime );

    tp->tv_sec = ltime;
    tp->tv_usec = 0;

}
#endif


/**
 *			C M D L I N E
 *
 *  This routine is called to process a vls full of commands.
 *  Each command is newline terminated.
 *  The input string will not be altered in any way.
 *
 *  Returns -
 *	!0	when a prompt needs to be printed.
 *	 0	no prompt needed.
 */
int
cmdline( struct bu_vls *vp, int record )
{
    int	status;
    struct bu_vls globbed;
    struct bu_vls tmp_vls;
    struct bu_vls save_vp;
    struct timeval start, finish;
    size_t len;
    extern struct bu_vls mged_prompt;
    char *cp;
    const char *result;

    BU_CK_VLS(vp);

    if (bu_vls_strlen(vp) <= 0)
	return CMD_OK;

    bu_vls_init(&globbed);
    bu_vls_init(&tmp_vls);
    bu_vls_init(&save_vp);
    bu_vls_vlscat(&save_vp, vp);

    /* MUST MAKE A BACKUP OF THE INPUT STRING AND USE THAT IN THE CALL TO
       Tcl_Eval!!!

       You never know who might change the string (append to it...)
       (f_mouse is notorious for adding things to the input string)
       If it were to change while it was still being evaluated, Horrible Things
       could happen.
    */

    if (glob_compat_mode) {
	mged_compat(&globbed, vp, 0);
    } else {
	bu_vls_vlscat(&globbed, vp);
    }

    gettimeofday(&start, (struct timezone *)NULL);
    status = Tcl_Eval(interp, bu_vls_addr(&globbed));
    gettimeofday(&finish, (struct timezone *)NULL);
    result = Tcl_GetStringResult(interp);

    /* Contemplate the result reported by the Tcl interpreter. */

    switch (status) {
	case TCL_RETURN:
	case TCL_OK:
	    if ( setjmp( jmp_env ) == 0 ) {
		len = strlen(result);

		/* If the command had something to say, print it out. */
		if (len > 0) {
		    (void)signal( SIGINT, sig3);  /* allow interupts */

		    bu_log("%s%s", result,
			   result[len-1] == '\n' ? "" : "\n");

		    (void)signal( SIGINT, SIG_IGN );
		}

		/* A user typed this command so let everybody see, then record
		   it in the history. */
		if (record && tkwin != NULL) {
		    bu_vls_printf(&tmp_vls, "distribute_text {} {%s} {%s}",
				  bu_vls_addr(&save_vp), result);
		    Tcl_Eval(interp, bu_vls_addr(&tmp_vls));
		    Tcl_SetResult(interp, "", TCL_STATIC);
		}

		if (record)
		    history_record(&save_vp, &start, &finish, CMD_OK);

	    } else {
/* XXXXXX */
		bu_semaphore_release(BU_SEM_SYSCALL);
		bu_log("\n");
	    }

	    bu_vls_strcpy(&mged_prompt, MGED_PROMPT);
	    status = CMD_OK;
	    goto end;

	case TCL_ERROR:
	default:

	    /* First check to see if it's a secret message. */

	    if ((cp = strstr(result, MORE_ARGS_STR)) != NULL) {
		if (cp == result) {
		    bu_vls_trunc(&mged_prompt, 0);
		    bu_vls_printf(&mged_prompt, "\r%s",
				  result+sizeof(MORE_ARGS_STR)-1);
		} else {
		    len = cp - result;
		    bu_log("%*s%s", len, result, result[len-1] == '\n' ? "" : "\n");
		    bu_vls_trunc(&mged_prompt, 0);
		    bu_vls_printf(&mged_prompt, "\r%s",
				  result+sizeof(MORE_ARGS_STR)-1+len);
		}

		status = CMD_MORE;
		goto end;
	    }

	    /* Otherwise, it's just a regular old error. */

	    len = strlen(result);
	    if (len > 0) bu_log("%s%s", result,
				result[len-1] == '\n' ? "" : "\n");

	    if (record)
		history_record(&save_vp, &start, &finish, CMD_BAD);

	    bu_vls_strcpy(&mged_prompt, MGED_PROMPT);
	    status = CMD_BAD;

	    /* Fall through to end */
    }

 end:
    bu_vls_free(&globbed);
    bu_vls_free(&tmp_vls);
    bu_vls_free(&save_vp);

    return status;
}


void
mged_print_result(int status)
{
    int len;
    extern void pr_prompt(void);
    const char *result = Tcl_GetStringResult(interp);

#if 0
    switch (status) {
	case TCL_OK:
	    len = strlen(result);

	    /* If the command had something to say, print it out. */
	    if (len > 0) {
		bu_log("%s%s", result,
		       result[len-1] == '\n' ? "" : "\n");

		pr_prompt();
	    }

	    break;

	case TCL_ERROR:
	default:
	    len = strlen(result);
	    if (len > 0) {
		bu_log("%s%s", result,
		       result[len-1] == '\n' ? "" : "\n");

		pr_prompt();
	    }

	    break;
    }
#else
    len = strlen(result);
    if (len > 0) {
	bu_log("%s%s", result,
	       result[len-1] == '\n' ? "" : "\n");

	pr_prompt();
    }
#endif

    Tcl_ResetResult(interp);
}

/**
 *			M G E D _ C M D
 *
 *  Check a table for the command, check for the correct minimum and maximum
 *  number of arguments, and pass control to the proper function.  If the
 *  number of arguments is incorrect, print out a short help message.
 */
int
mged_cmd(
    int argc,
    char **argv,
    struct funtab in_functions[])
{
    register struct funtab *ftp;
    struct funtab *functions;

    if (argc == 0)
	return CMD_OK;	/* No command entered, that's fine */

    /* if no function table is provided, use the default mged function table */
    if ( in_functions == (struct funtab *)NULL )
    {
	bu_log("mged_cmd: failed to supply function table!\n");
	return CMD_BAD;
    }
    else
	functions = in_functions;

    for (ftp = &functions[1]; ftp->ft_name; ftp++) {
	if (strcmp(ftp->ft_name, argv[0]) != 0)
	    continue;
	/* We have a match */
	if ((ftp->ft_min <= argc) && (argc <= ftp->ft_max)) {
	    /* Input has the right number of args.
	     * Call function listed in table, with
	     * main(argc, argv) style args
	     */

	    switch (ftp->ft_func(argc, argv)) {
		case CMD_OK:
		    return CMD_OK;
		case CMD_BAD:
		    return CMD_BAD;
		case CMD_MORE:
		    return CMD_MORE;
		default:
		    Tcl_AppendResult(interp, "mged_cmd(): Invalid return from ",
				     ftp->ft_name, "\n", (char *)NULL);
		    return CMD_BAD;
	    }
	}

	Tcl_AppendResult(interp, "Usage: ", functions[0].ft_name, ftp->ft_name,
			 " ", ftp->ft_parms, "\n\t(", ftp->ft_comment,
			 ")\n", (char *)NULL);
	return CMD_BAD;
    }

    Tcl_AppendResult(interp, functions[0].ft_name, argv[0],
		     ": no such command, type '", functions[0].ft_name,
		     "?' for help\n", (char *)NULL);
    return CMD_BAD;
}

/**
 * Let the user temporarily escape from the editor
 * Format: %
 */
int
f_comm(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{


    register int pid, rpid;
    int retcode;

    if (argc != 1 || !classic_mged || curr_cmd_list != &head_cmd_list) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "help %s", argv[0]);
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

#ifndef _WIN32
    (void)signal( SIGINT, SIG_IGN );
    if ( ( pid = fork()) == 0 )  {
	(void)signal( SIGINT, SIG_DFL );
	(void)execl("/bin/sh", "-", (char *)NULL);
	perror("/bin/sh");
	mged_finish( 11 );
    }

    while ((rpid = wait(&retcode)) != pid && rpid != -1)
	;
#endif

    Tcl_AppendResult(interp, "Returning to MGED\n", (char *)NULL);

    return TCL_OK;
}

/**
 * Quit and exit gracefully
 * Format: q
 */
int
f_quit(
    ClientData clientData,
    Tcl_Interp *interp,
    int	argc,
    char	**argv)
{
    if (argc < 1 || 1 < argc) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "help %s", argv[0]);
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    if ( state != ST_VIEW )
	button( BE_REJECT );

    quit();			/* Exiting time */
    /* NOTREACHED */
    return TCL_OK;
}

/**
 * sync the database to disk
 */
int
f_sync(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{

    if (argc < 1 || 1 < argc) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "help %s", argv[0]);
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    if (dbip != DBI_NULL) {
	db_sync(dbip);
    }

    return TCL_OK;
}

/**
 *			H E L P C O M M
 *
 *  Common code for help commands
 */
HIDDEN int
helpcomm(int argc, char **argv, struct funtab *functions)
{
    register struct funtab *ftp;
    register int	i, bad;

    bad = 0;

    /* Help command(s) */
    for ( i=1; i<argc; i++ )  {
	for ( ftp = functions+1; ftp->ft_name; ftp++ )  {
	    if ( strcmp( ftp->ft_name, argv[i] ) != 0 )
		continue;

	    Tcl_AppendResult(interp, "Usage: ", functions->ft_name, ftp->ft_name,
			     " ", ftp->ft_parms, "\n\t(", ftp->ft_comment, ")\n", (char *)NULL);
	    break;
	}
	if ( !ftp->ft_name ) {
	    Tcl_AppendResult(interp, functions->ft_name, argv[i],
			     ": no such command, type '", functions->ft_name,
			     "?' for help\n", (char *)NULL);
	    bad = 1;
	}
    }

    return bad ? TCL_ERROR : TCL_OK;
}

/**
 *			F _ H E L P
 *
 *  Print a help message, two lines for each command.
 *  Or, help with the indicated commands.
 */
int
f_help2(int argc, char **argv, struct funtab *functions)
{
    register struct funtab *ftp;

    if ( argc <= 1 )  {
	Tcl_AppendResult(interp, "The following commands are available:\n", (char *)NULL);
	for ( ftp = functions+1; ftp->ft_name; ftp++ )  {
	    Tcl_AppendResult(interp,  functions->ft_name, ftp->ft_name, " ",
			     ftp->ft_parms, "\n\t(", ftp->ft_comment, ")\n", (char *)NULL);
	}
	return TCL_OK;
    }
    return helpcomm( argc, argv, functions );
}

int
f_fhelp2(int argc, char **argv, struct funtab *functions)
{
    register struct funtab *ftp;
    struct bu_vls		str;

    if ( argc <= 1 )  {
	bu_vls_init(&str);
	Tcl_AppendResult(interp, "The following ", functions->ft_name,
			 " commands are available:\n", (char *)NULL);
	for ( ftp = functions+1; ftp->ft_name; ftp++ )  {
	    vls_col_item( &str, ftp->ft_name);
	}
	vls_col_eol( &str );
	Tcl_AppendResult(interp, bu_vls_addr( &str ), (char *)NULL);
	bu_vls_free(&str);
	return TCL_OK;
    }
    return helpcomm( argc, argv, functions );
}

int
cmd_summary(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_summary_cmd(wdbp, interp, argc, argv);
}

/**
 *                          C M D _ E C H O
 *
 * Concatenates its arguments and "bu_log"s the resulting string.
 */
int
cmd_echo(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    register int i;

    if (argc < 1) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "help echo");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    for ( i=1; i < argc; i++ )  {
	Tcl_AppendResult(interp, i==1 ? "" : " ", argv[i], (char *)NULL);
    }

    Tcl_AppendResult(interp, "\n", (char *)NULL);

    return TCL_OK;
}


/**
 * SYNOPSIS
 *	tie [cw [dm]]
 *	tie -u cw
 *
 * DESCRIPTION
 *	This command ties/associates a command window (cw) to a display manager window (dm).
 *	When a command window is tied to a display manager window, all commands issued from
 *	this window will be directed at a particular display manager. Otherwise, the
 *	commands issued will be directed at the current display manager window.
 *
 * EXAMPLES
 *	tie		--->	returns a list of the command_window/display_manager associations
 *	tie cw1		--->	returns the display_manager, if it exists, associated with cw1
 *	tie cw1 dm1	--->	associated cw1 with dm1
 *	tie -u cw1	--->	removes the association, if it exists, cw1 has with a display manager
 */
int
f_tie(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    register int uflag = 0;		/* untie flag */
    struct cmd_list *clp;
    struct dm_list *dlp;
    struct bu_vls vls;

    bu_vls_init(&vls);

    if (argc < 1 || 3 < argc) {
	bu_vls_printf(&vls, "helpdevel tie");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    if (argc == 1) {
	for ( BU_LIST_FOR(clp, cmd_list, &head_cmd_list.l) ) {
	    bu_vls_trunc(&vls, 0);
	    if (clp->cl_tie) {
		bu_vls_printf(&vls, "%S %S", &clp->cl_name,
			      &clp->cl_tie->dml_dmp->dm_pathName);
		Tcl_AppendElement(interp, bu_vls_addr(&vls));
	    } else {
		bu_vls_printf(&vls, "%S {}", &clp->cl_name);
		Tcl_AppendElement(interp, bu_vls_addr(&vls));
	    }
	}

	bu_vls_trunc(&vls, 0);
	if (clp->cl_tie) {
	    bu_vls_printf(&vls, "%S %S", &clp->cl_name,
			  &clp->cl_tie->dml_dmp->dm_pathName);
	    Tcl_AppendElement(interp, bu_vls_addr(&vls));
	} else {
	    bu_vls_printf(&vls, "%S {}", &clp->cl_name);
	    Tcl_AppendElement(interp, bu_vls_addr(&vls));
	}

	bu_vls_free(&vls);
	return TCL_OK;
    }

    if (argv[1][0] == '-' && argv[1][1] == 'u') {
	uflag = 1;
	--argc;
	++argv;
    }

    if (argc < 2) {
	bu_vls_printf(&vls, "help tie");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    for ( BU_LIST_FOR(clp, cmd_list, &head_cmd_list.l) )
	if (!strcmp(bu_vls_addr(&clp->cl_name), argv[1]))
	    break;

    if (clp == &head_cmd_list &&
	(strcmp(bu_vls_addr(&head_cmd_list.cl_name), argv[1]))) {
	Tcl_AppendResult(interp, "f_tie: unrecognized command_window - ", argv[1],
			 "\n", (char *)NULL);
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    if (uflag) {
	if (clp->cl_tie)
	    clp->cl_tie->dml_tie = (struct cmd_list *)NULL;

	clp->cl_tie = (struct dm_list *)NULL;

	bu_vls_free(&vls);
	return TCL_OK;
    }

    /* print out the display manager that we're tied to */
    if (argc == 2) {
	if (clp->cl_tie)
	    Tcl_AppendElement(interp, bu_vls_addr(&clp->cl_tie->dml_dmp->dm_pathName));
	else
	    Tcl_AppendElement(interp, "");

	bu_vls_free(&vls);
	return TCL_OK;
    }

    if (*argv[2] != '.')
	bu_vls_printf(&vls, ".%s", argv[2]);
    else
	bu_vls_strcpy(&vls, argv[2]);

    FOR_ALL_DISPLAYS(dlp, &head_dm_list.l)
	if (!bu_vls_strcmp(&vls, &dlp->dml_dmp->dm_pathName))
	    break;

    if (dlp == &head_dm_list) {
	Tcl_AppendResult(interp, "f_tie: unrecognized pathName - ",
			 bu_vls_addr(&vls), "\n", (char *)NULL);
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    /* already tied */
    if (clp->cl_tie)
	clp->cl_tie->dml_tie = (struct cmd_list *)NULL;

    clp->cl_tie = dlp;

    /* already tied */
    if (dlp->dml_tie)
	dlp->dml_tie->cl_tie = (struct dm_list *)NULL;

    dlp->dml_tie = clp;

    bu_vls_free(&vls);
    return TCL_OK;
}

int
f_ps(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    int status;
    char *av[2];
    struct dm_list *dml;
    struct _view_state *vsp;

    if (argc < 2) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "help ps");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    dml = curr_dm_list;
    status = mged_attach(&which_dm[DM_PS_INDEX], argc, argv);
    if (status == TCL_ERROR)
	return TCL_ERROR;

    vsp = view_state;  /* save state info pointer */

    bu_free((genptr_t)menu_state, "f_ps: menu_state");
    menu_state = dml->dml_menu_state;

    scroll_top = dml->dml_scroll_top;
    scroll_active = dml->dml_scroll_active;
    scroll_y = dml->dml_scroll_y;
    memmove((void *)scroll_array, (void *)dml->dml_scroll_array, sizeof(struct scroll_item *) * 6);

    dirty = 1;
    refresh();

    view_state = vsp;  /* restore state info pointer */
    av[0] = "release";
    av[1] = NULL;
    status = f_release(clientData, interp, 1, av);
    curr_dm_list = dml;

    return status;
}

/**
 * Experimental - like f_plot except we attach to dm-plot, passing along
 *                any arguments.
 */
int
f_pl(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    int status;
    char *av[2];
    struct dm_list *dml;
    struct _view_state *vsp;

    if (argc < 2) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "help pl");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    dml = curr_dm_list;
    status = mged_attach(&which_dm[DM_PLOT_INDEX], argc, argv);
    if (status == TCL_ERROR)
	return TCL_ERROR;

    vsp = view_state;  /* save state info pointer */
    view_state = dml->dml_view_state;  /* use dml's state info */
    *mged_variables = *dml->dml_mged_variables; /* struct copy */

    bu_free((genptr_t)menu_state, "f_pl: menu_state");
    menu_state = dml->dml_menu_state;

    scroll_top = dml->dml_scroll_top;
    scroll_active = dml->dml_scroll_active;
    scroll_y = dml->dml_scroll_y;
    memmove((void *)scroll_array, (void *)dml->dml_scroll_array, sizeof(struct scroll_item *) * 6);

    dirty = 1;
    refresh();

    view_state = vsp;  /* restore state info pointer */
    av[0] = "release";
    av[1] = NULL;
    status = f_release(clientData, interp, 1, av);
    curr_dm_list = dml;

    return status;
}

int
f_winset(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    register struct dm_list *p;

    if (argc < 1 || 2 < argc) {
	struct bu_vls vls;

	bu_vls_init(&vls);
	bu_vls_printf(&vls, "helpdevel winset");
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);
	return TCL_ERROR;
    }

    /* print pathname of drawing window with primary focus */
    if ( argc == 1 ) {
	Tcl_AppendResult(interp, bu_vls_addr(&pathName), (char *)NULL);
	return TCL_OK;
    }

    /* change primary focus to window argv[1] */
    FOR_ALL_DISPLAYS(p, &head_dm_list.l) {
	if ( !strcmp( argv[1], bu_vls_addr( &p->dml_dmp->dm_pathName ) ) ) {
	    curr_dm_list = p;

	    if (curr_dm_list->dml_tie)
		curr_cmd_list = curr_dm_list->dml_tie;
	    else
		curr_cmd_list = &head_cmd_list;

	    return TCL_OK;
	}
    }

    Tcl_AppendResult(interp, "Unrecognized pathname - ", argv[1],
		     "\n", (char *)NULL);
    return TCL_ERROR;
}

void
mged_global_variable_setup(Tcl_Interp *interp)
{
    Tcl_LinkVar(interp, "mged_default(dlist)", (char *)&mged_default_dlist, TCL_LINK_INT);
    Tcl_LinkVar(interp, "mged_default(db_warn)", (char *)&db_warn, TCL_LINK_INT);
    Tcl_LinkVar(interp, "mged_default(db_upgrade)", (char *)&db_upgrade, TCL_LINK_INT);
    Tcl_LinkVar(interp, "mged_default(db_version)", (char *)&db_version, TCL_LINK_INT);

    Tcl_LinkVar(interp, "edit_class", (char *)&es_edclass, TCL_LINK_INT);
    Tcl_LinkVar(interp, "edit_solid_flag", (char *)&es_edflag, TCL_LINK_INT);
    Tcl_LinkVar(interp, "edit_object_flag", (char *)&edobj, TCL_LINK_INT);
}


void
mged_global_variable_teardown(Tcl_Interp *interp)
{
    Tcl_UnlinkVar(interp, "mged_default(dlist)");
    Tcl_UnlinkVar(interp, "mged_default(db_warn)");
    Tcl_UnlinkVar(interp, "mged_default(db_upgrade)");
    Tcl_UnlinkVar(interp, "mged_default(db_version)");

    Tcl_UnlinkVar(interp, "edit_class");
    Tcl_UnlinkVar(interp, "edit_solid_flag");
    Tcl_UnlinkVar(interp, "edit_object_flag");
}


int
f_bot_split(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    struct directory *dp;
    struct rt_db_internal intern;
    struct rt_bot_internal **bots;
    struct rt_bot_internal *bot;
    int bot_count = 256;
    int edge, e, f;
    int * edges;
    int face;


    CHECK_DBI_NULL;
    CHECK_READ_ONLY;

    if (argc < 2) {
	Tcl_AppendResult(interp, "Usage:\nbot_merge bot_dest bot1_src [botn_src]\n", (char *)NULL );
	return TCL_ERROR;
    }


    bots = bu_calloc(sizeof(struct rt_bot_internal), bot_count, "bot internal");

    if ((dp = db_lookup(dbip, argv[1], LOOKUP_NOISY)) == DIR_NULL) {
	return TCL_ERROR;
    }

    if ( rt_db_get_internal( &intern, dp, dbip, bn_mat_identity, &rt_uniresource ) < 0 ) {
	Tcl_AppendResult(interp, "rt_db_get_internal(", argv[1], ") error\n", (char *)NULL);
	return TCL_ERROR;

    }

    if ( intern.idb_type != ID_BOT ) 	{
	Tcl_AppendResult(interp, argv[1], " is not a BOT solid!!!  skipping\n", (char *)NULL );
	return TCL_ERROR;
    }

    bot = (struct rt_bot_internal *)intern.idb_ptr;
    edges = bu_calloc(bot->num_faces, 3, "num_edges");


    for (face=0; face < bot->num_faces; face++) {
	int *faceptr = &bot->faces[face*3];

	for (edge=0; edge < 3; edge++) {

	    for (f=face+1; f < bot->num_faces; f++) {
		int *fptr = &bot->faces[f*3];

		for (e=0; e < 3; e++) {
		    /* does e match edge? */

		    if ( (fptr[e] == faceptr[edge] && fptr[ (e+1) % 3 ] == faceptr[ (edge+1) % 3 ]) ||
			 (fptr[e] == faceptr[ (edge+1) % 3 ] && fptr[ (e+1) % 3 ] == faceptr[edge]) ) {
			/* edge match */
			edges[face*3+edge]++;
			edges[face*3+edge]++;
		    }


		}
	    }
	}
    }
    return TCL_OK;
}


int
f_bot_merge(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    struct directory *dp, *new_dp;
    struct rt_db_internal intern;
    struct rt_bot_internal **bots;
    int i, idx, retval;
    int avail_vert, avail_face, face;


    CHECK_DBI_NULL;
    CHECK_READ_ONLY;

    if (argc < 2) {
	Tcl_AppendResult(interp, "Usage:\nbot_merge bot_dest bot1_src [botn_src]\n", (char *)NULL );
	return TCL_ERROR;
    }

    bots = bu_calloc(sizeof(struct rt_bot_internal), argc, "bot internal");

    retval = TCL_OK;


    /* create a new bot */
    BU_GETSTRUCT(bots[0], rt_bot_internal);
    bots[0]->mode = 0;
    bots[0]->orientation = RT_BOT_UNORIENTED;
    bots[0]->bot_flags = 0;
    bots[0]->num_vertices = 0;
    bots[0]->num_faces = 0;
    bots[0]->faces = (int *)0;
    bots[0]->vertices = (fastf_t *)0;
    bots[0]->thickness = (fastf_t *)0;
    bots[0]->face_mode = (struct bu_bitv*)0;
    bots[0]->num_normals = 0;
    bots[0]->normals = (fastf_t *)0;
    bots[0]->num_face_normals = 0;
    bots[0]->face_normals = 0;
    bots[0]->magic = RT_BOT_INTERNAL_MAGIC;


    /* read in all the bots */
    for (idx=1, i=2; i < argc; i++ ) {
	if ((dp = db_lookup(dbip, argv[i], LOOKUP_NOISY)) == DIR_NULL) {
	    continue;
	}

	if ( rt_db_get_internal( &intern, dp, dbip, bn_mat_identity, &rt_uniresource ) < 0 ) {
	    Tcl_AppendResult(interp, "rt_db_get_internal(", argv[i], ") error\n", (char *)NULL);
	    retval = TCL_ERROR;
	    continue;
	}

	if ( intern.idb_type != ID_BOT ) 	{
	    Tcl_AppendResult(interp, argv[i], " is not a BOT solid!!!  skipping\n", (char *)NULL );
	    retval = TCL_ERROR;
	    continue;
	}

	bots[idx] = (struct rt_bot_internal *)intern.idb_ptr;

	intern.idb_ptr = (genptr_t)0;

	RT_BOT_CK_MAGIC( bots[idx] );

	bots[0]->num_vertices += bots[idx]->num_vertices;
	bots[0]->num_faces += bots[idx]->num_faces;

	idx++;
    }

    if (idx == 1) return TCL_ERROR;


    for (i=1; i < idx; i++ ) {
	/* check for surface normals */
	if (bots[0]->mode) {
	    if (bots[0]->mode != bots[i]->mode) {
		bu_log("Warning: not all bots share same mode\n");
	    }
	} else {
	    bots[0]->mode = bots[i]->mode;
	}


	if (bots[i]->bot_flags & RT_BOT_HAS_SURFACE_NORMALS) bots[0]->bot_flags |= RT_BOT_HAS_SURFACE_NORMALS;
	if (bots[i]->bot_flags & RT_BOT_USE_NORMALS) bots[0]->bot_flags |= RT_BOT_USE_NORMALS;

	if (bots[0]->orientation) {
	    if (bots[i]->orientation == RT_BOT_UNORIENTED) {
		bots[0]->orientation = RT_BOT_UNORIENTED;
	    } else {
		bots[i]->magic = 1; /* set flag to reverse order of faces */
	    }
	} else {
	    bots[0]->orientation = bots[i]->orientation;
	}
    }


    bots[0]->vertices = bu_calloc(bots[0]->num_vertices*3, sizeof(fastf_t), "verts");
    bots[0]->faces = bu_calloc(bots[0]->num_faces*3, sizeof(int), "verts");

    avail_vert = 0;
    avail_face = 0;


    for (i=1; i < idx; i++ ) {
	/* copy the vertices */
	memcpy(&bots[0]->vertices[3*avail_vert], bots[i]->vertices, bots[i]->num_vertices*3*sizeof(fastf_t));

	/* copy/convert the faces, potentially maintaining a common orientation */
	if (bots[0]->orientation != RT_BOT_UNORIENTED && bots[i]->magic != RT_BOT_INTERNAL_MAGIC) {
	    /* copy and reverse */
	    for (face=0; face < bots[i]->num_faces; face++) {
		/* copy the 3 verts of this face and convert to new index */
		bots[0]->faces[avail_face*3+face*3+2] = bots[i]->faces[face*3  ] + avail_vert;
		bots[0]->faces[avail_face*3+face*3+1] = bots[i]->faces[face*3+1] + avail_vert;
		bots[0]->faces[avail_face*3+face*3  ] = bots[i]->faces[face*3+2] + avail_vert;
	    }
	} else {
	    /* just copy */
	    for (face=0; face < bots[i]->num_faces; face++) {
		/* copy the 3 verts of this face and convert to new index */
		bots[0]->faces[avail_face*3+face*3  ] = bots[i]->faces[face*3  ] + avail_vert;
		bots[0]->faces[avail_face*3+face*3+1] = bots[i]->faces[face*3+1] + avail_vert;
		bots[0]->faces[avail_face*3+face*3+2] = bots[i]->faces[face*3+2] + avail_vert;
	    }
	}

	/* copy surface normals */
	if (bots[0]->bot_flags == RT_BOT_HAS_SURFACE_NORMALS) {
	    bu_log("not yet copying surface normals\n");
	    if (bots[i]->bot_flags == RT_BOT_HAS_SURFACE_NORMALS) {
	    } else {
	    }
	}


	avail_vert += bots[i]->num_vertices;
	avail_face += bots[i]->num_faces;
    }

    RT_INIT_DB_INTERNAL(&intern);
    intern.idb_type = ID_BOT;
    intern.idb_major_type = DB5_MAJORTYPE_BRLCAD;
    intern.idb_minor_type = DB5_MINORTYPE_BRLCAD_BOT;
    intern.idb_meth = &rt_functab[ID_BOT];
    intern.idb_ptr = (genptr_t)bots[0];

    if ( (new_dp=db_diradd( dbip, argv[1], -1L, 0, DIR_SOLID, (genptr_t)&intern.idb_type)) == DIR_NULL )
    {
	Tcl_AppendResult(interp, "Cannot add ", argv[1], " to directory\n", (char *)NULL );
	return TCL_ERROR;
    }

    if ( rt_db_put_internal( new_dp, dbip, &intern, &rt_uniresource ) < 0 )
    {
	rt_db_free_internal( &intern, &rt_uniresource );
	TCL_WRITE_ERR_return;
    }

    bu_free(bots, "bots");

    return retval;
}
int
f_bot_face_fuse(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    struct directory *old_dp, *new_dp;
    struct rt_db_internal intern;
    struct rt_bot_internal *bot;

    CHECK_DBI_NULL;
    CHECK_READ_ONLY;

    if (argc != 3) {
	Tcl_AppendResult(interp, "Usage:\nbot_face_fuse new_bot_solid old_bot_solid\n", (char *)NULL );
	return TCL_ERROR;
    }

    if ( (old_dp = db_lookup( dbip, argv[2], LOOKUP_NOISY )) == DIR_NULL )
	return TCL_ERROR;

    if ( rt_db_get_internal( &intern, old_dp, dbip, bn_mat_identity, &rt_uniresource ) < 0 )
    {
	Tcl_AppendResult(interp, "rt_db_get_internal() error\n", (char *)NULL);
	return TCL_ERROR;
    }

    if ( intern.idb_type != ID_BOT )
    {
	Tcl_AppendResult(interp, argv[2], " is not a BOT solid!!!\n", (char *)NULL );
	return TCL_ERROR;
    }

    bot = (struct rt_bot_internal *)intern.idb_ptr;
    RT_BOT_CK_MAGIC( bot );

    (void) rt_bot_face_fuse( bot );

    if ( (new_dp=db_diradd( dbip, argv[1], -1L, 0, DIR_SOLID, (genptr_t)&intern.idb_type)) == DIR_NULL )
    {
	Tcl_AppendResult(interp, "Cannot add ", argv[1], " to directory\n", (char *)NULL );
	return TCL_ERROR;
    }

    if ( rt_db_put_internal( new_dp, dbip, &intern, &rt_uniresource ) < 0 )
    {
	rt_db_free_internal( &intern, &rt_uniresource );
	TCL_WRITE_ERR_return;
    }
    return TCL_OK;
}

int
f_bot_fuse(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    struct directory *old_dp, *new_dp;
    struct rt_db_internal intern;
    struct rt_bot_internal *bot;
    int count1=0;

    CHECK_DBI_NULL;
    CHECK_READ_ONLY;

    if (argc != 3) {
	Tcl_AppendResult(interp, "Usage:\nbot_fuse new_bot_solid old_bot_solid\n", (char *)NULL );
	return TCL_ERROR;
    }

    if ( (old_dp = db_lookup( dbip, argv[2], LOOKUP_NOISY )) == DIR_NULL )
	return TCL_ERROR;

    if ( rt_db_get_internal( &intern, old_dp, dbip, bn_mat_identity, &rt_uniresource ) < 0 )
    {
	Tcl_AppendResult(interp, "rt_db_get_internal() error\n", (char *)NULL);
	return TCL_ERROR;
    }

    if ( intern.idb_type != ID_BOT )
    {
	Tcl_AppendResult(interp, argv[2], " is not a BOT solid!!!\n", (char *)NULL );
	return TCL_ERROR;
    }

    bot = (struct rt_bot_internal *)intern.idb_ptr;
    RT_BOT_CK_MAGIC( bot );

    count1 = rt_bot_vertex_fuse( bot );
    if ( count1 )
	(void)rt_bot_condense( bot );

    if ( (new_dp=db_diradd( dbip, argv[1], -1L, 0, DIR_SOLID, (genptr_t)&intern.idb_type)) == DIR_NULL )
    {
	Tcl_AppendResult(interp, "Cannot add ", argv[1], " to directory\n", (char *)NULL );
	return TCL_ERROR;
    }

    if ( rt_db_put_internal( new_dp, dbip, &intern, &rt_uniresource ) < 0 )
    {
	rt_db_free_internal( &intern, &rt_uniresource );
	TCL_WRITE_ERR_return;
    }
    return TCL_OK;
}

int
f_bot_condense(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    struct directory *old_dp, *new_dp;
    struct rt_db_internal intern;
    struct rt_bot_internal *bot;
    int count2=0;
    char count_str[255];

    CHECK_DBI_NULL;
    CHECK_READ_ONLY;

    if (argc != 3) {
	Tcl_AppendResult(interp, "Usage:\nbot_condense new_bot_solid old_bot_solid\n", (char *)NULL );
	return TCL_ERROR;
    }

    if ( (old_dp = db_lookup( dbip, argv[2], LOOKUP_NOISY )) == DIR_NULL )
	return TCL_ERROR;

    if ( rt_db_get_internal( &intern, old_dp, dbip, bn_mat_identity, &rt_uniresource ) < 0 )
    {
	Tcl_AppendResult(interp, "rt_db_get_internal() error\n", (char *)NULL);
	return TCL_ERROR;
    }

    if ( intern.idb_type != ID_BOT )
    {
	Tcl_AppendResult(interp, argv[2], " is not a BOT solid!!!\n", (char *)NULL );
	return TCL_ERROR;
    }

    bot = (struct rt_bot_internal *)intern.idb_ptr;
    RT_BOT_CK_MAGIC( bot );

    count2 = rt_bot_condense( bot );
    sprintf( count_str, "%d", count2 );
    Tcl_AppendResult(interp, count_str, " dead vertices eliminated\n", (char *)NULL );

    if ( (new_dp=db_diradd( dbip, argv[1], -1L, 0, DIR_SOLID, (genptr_t)&intern.idb_type)) == DIR_NULL )
    {
	Tcl_AppendResult(interp, "Cannot add ", argv[1], " to directory\n", (char *)NULL );
	return TCL_ERROR;
    }

    if ( rt_db_put_internal( new_dp, dbip, &intern, &rt_uniresource ) < 0 )
    {
	rt_db_free_internal( &intern, &rt_uniresource );
	TCL_WRITE_ERR_return;
    }
    return TCL_OK;
}

int
f_bomb(ClientData clientData, Tcl_Interp *interp, int argc, const char *argv[])
{
    char buffer[1024] = {0};
    struct bu_vls vls;

    if (argc > 1) {
	argc--; argv++;

	bu_vls_init(&vls);
	bu_vls_from_argv(&vls, argc, argv);
	snprintf(buffer, 1024, "%s", bu_vls_addr(&vls));
	bu_vls_free(&vls);
    }

    bu_exit(EXIT_FAILURE, buffer);

    /* This is never reached */

    return TCL_OK;
}

int
cmd_adjust(ClientData	clientData,
	   Tcl_Interp	*interp,
	   int		argc,
	   char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_adjust_cmd(wdbp, interp, argc, argv);
}

int
cmd_attr(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_attr_cmd(wdbp, interp, argc, argv);
}

int
cmd_dbip(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_dbip_cmd(wdbp, interp, argc, argv);
}

int
cmd_dump(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_dump_cmd(wdbp, interp, argc, argv);
}

int
cmd_form(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_form_cmd(wdbp, interp, argc, argv);
}

int
cmd_get(ClientData	clientData,
	Tcl_Interp	*interp,
	int		argc,
	char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_get_cmd(wdbp, interp, argc, argv);
}

int
cmd_match(ClientData	clientData,
	  Tcl_Interp	*interp,
	  int		argc,
	  char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_match_cmd(wdbp, interp, argc, argv);
}

int
cmd_put(ClientData	clientData,
	Tcl_Interp	*interp,
	int		argc,
	char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_put_cmd(wdbp, interp, argc, argv);
}

int
cmd_rt_gettrees(ClientData	clientData,
		Tcl_Interp	*interp,
		int		argc,
		char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_rt_gettrees_cmd(wdbp, interp, argc, argv);
}

int
cmd_dbversion(ClientData	clientData,
	      Tcl_Interp	*interp,
	      int		argc,
	      char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_version_cmd(wdbp, interp, argc, argv);
}

/**
 *		    C M D _ C O M B _ S T D
 *
 *	Input a combination in standard set-theoetic notation
 *
 *	Syntax: c [-gr] comb_name <boolean_expr>
 */
int
cmd_comb_std(ClientData	clientData,
	     Tcl_Interp	*interp,
	     int	argc,
	     char	**argv)
{
    CHECK_DBI_NULL;

    return wdb_comb_std_cmd(wdbp, interp, argc, argv);
}

int
cmd_nmg_collapse(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    const char *av[3];

    CHECK_DBI_NULL;

    if (wdb_nmg_collapse_cmd(wdbp, interp, argc, argv) == TCL_ERROR)
	return TCL_ERROR;

    av[0] = "e";
    av[1] = argv[2];
    av[2] = NULL;

    return cmd_draw(clientData, interp, 2, av);
}

/**
 *			F _ M A K E _ N A M E
 *
 * Generate an identifier that is guaranteed not to be the name
 * of any object currently in the database.
 *
 */
int
cmd_make_name(ClientData	clientData,
	      Tcl_Interp	*interp,
	      int		argc,
	      char		**argv)
{
    Tcl_DString ds;
    int ret;

    CHECK_DBI_NULL;

    ret = ged_make_name(wdbp, argc, argv);

    /* Convert to Tcl codes */
    if (ret == GED_OK)
	ret = TCL_OK;
    else
	ret = TCL_ERROR;

    Tcl_DStringInit(&ds);
    Tcl_DStringAppend(&ds, bu_vls_addr(&wdbp->wdb_result_str), -1);
    Tcl_DStringResult(interp, &ds);

    return ret;
}

int
cmd_shells(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_shells_cmd(wdbp, interp, argc, argv);
}

/**
 *  	F _ P A T H S U M :   does the following
 *		1.  produces path for purposes of matching
 *      	2.  gives all paths matching the input path OR
 *		3.  gives a summary of all paths matching the input path
 *		    including the final parameters of the solids at the bottom
 *		    of the matching paths
 */
int
cmd_pathsum(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    int	ret;

    CHECK_DBI_NULL;

    if (argc < 2) {
	/* get the path */
	Tcl_AppendResult(interp, MORE_ARGS_STR,
			 "Enter the path: ", (char *)NULL);
	return TCL_ERROR;
    }

#if 0
    if (setjmp(jmp_env) == 0)
	(void)signal(SIGINT, sig3);  /* allow interupts */
    else
	return TCL_OK;
#endif

    ret = wdb_pathsum_cmd(wdbp, interp, argc, argv);

#if 0
    (void)signal( SIGINT, SIG_IGN );
#endif
    return ret;
}

/**
 *   	F _ C O P Y E V A L : copys an evaluated solid
 */
int
cmd_copyeval(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    int ret;

    CHECK_DBI_NULL;

    if (argc < 3) {
	Tcl_AppendResult(interp, MORE_ARGS_STR,
			 "Enter new_solid_name and full path to old_solid\n",
			 (char *)NULL);
	return TCL_ERROR;
    }

    if (setjmp(jmp_env) == 0)
	(void)signal(SIGINT, sig3);  /* allow interupts */
    else
	return TCL_OK;

    ret = wdb_copyeval_cmd(wdbp, interp, argc, argv);

    (void)signal( SIGINT, SIG_IGN );
    return ret;
}


/**
 *			F _ P U S H
 *
 * The push command is used to move matrices from combinations
 * down to the solids. At some point, it is worth while thinking
 * about adding a limit to have the push go only N levels down.
 *
 * the -d flag turns on the treewalker debugging output.
 * the -P flag allows for multi-processor tree walking (not useful)
 * the -l flag is there to select levels even if it does not currently work.
 */
int
cmd_push(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_push_cmd(wdbp, interp, argc, argv);
}

int
cmd_hide(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_hide_cmd(wdbp, interp, argc, argv);
}

int
cmd_unhide(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_unhide_cmd(wdbp, interp, argc, argv);
}

int
cmd_xpush(ClientData	clientData,
	  Tcl_Interp	*interp,
	  int		argc,
	  char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_xpush_cmd(wdbp, interp, argc, argv);
}

int
cmd_showmats(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_showmats_cmd(wdbp, interp, argc, argv);
}

int
cmd_nmg_simplify(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_nmg_simplify_cmd(wdbp, interp, argc, argv);
}

/**
 *			F _ M A K E _ B B
 *
 *	Build an RPP bounding box for the list of objects and/or paths passed to this routine
 */
int
cmd_make_bb(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_make_bb_cmd(wdbp, interp, argc, argv);
}

int
cmd_whatid(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_whatid_cmd(wdbp, interp, argc, argv);
}

/**
 *      C M D _ W H I C H
 *
 *	Finds all regions with given region ids or air codes.
 */
int
cmd_which(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    int		ret;

    CHECK_DBI_NULL;

    if (setjmp(jmp_env) == 0)
	(void)signal(SIGINT, sig3);  /* allow interupts */
    else
	return TCL_OK;

    ret = wdb_which_cmd(wdbp, interp, argc, argv);

    (void)signal(SIGINT, SIG_IGN);
    return ret;
}

/**
 *  			C M D _ T O P S
 *
 *  Find all top level objects.
 *  TODO:  Perhaps print all objects, sorted by use count, as an option?
 */
int
cmd_tops(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    int		ret;

    CHECK_DBI_NULL;

    if (setjmp(jmp_env) == 0)
	(void)signal(SIGINT, sig3);  /* allow interupts */
    else
	return TCL_OK;

    ret = wdb_tops_cmd(wdbp, interp, argc, argv);

    (void)signal(SIGINT, SIG_IGN);
    return ret;
}

/**
 *			C M D _ T R E E
 *
 *	Print out a list of all members and submembers of an object.
 */
int
cmd_tree(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    int		ret;

    CHECK_DBI_NULL;

#if 0
    if (setjmp(jmp_env) == 0)
	(void)signal(SIGINT, sig3);  /* allow interupts */
    else
	return TCL_OK;
#endif

    /*
     * The tree command is wrapped by tclscripts/tree.tcl and calls this
     * routine with the name _mged_tree. So, we put back the original name.
     */
    argv[0] = "tree";
    ret = dgo_tree_cmd(dgop, interp, argc, argv);

#if 0
    (void)signal(SIGINT, SIG_IGN);
#endif
    return ret;
}

/**
 *	C M D _ M V A L L
 *
 *	rename all occurences of an object
 *	format:	mvall oldname newname
 *
 */
int
cmd_mvall(ClientData	clientData,
	  Tcl_Interp	*interp,
	  int		argc,
	  char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_move_all_cmd(wdbp, interp, argc, argv);
}

/**
 *			C M D _ D U P
 *
 *  Check for duplicate names in preparation for cat'ing of files
 *
 *  Usage:  dup file.g [prefix]
 *  becomes: db dup file.g [prefix]
 */
int
cmd_dup(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_dup_cmd(wdbp, interp, argc, argv);
}

/**
 *			C M D _ C O N C A T
 *
 *  Concatenate another GED file into the current file.
 *  Interrupts are not permitted during this function.
 *
 *  Usage:  dbconcat [-p|-s] file.g [prefix|suffix]
 *  becomes: db concat file.g prefix
 *
 *  NOTE:  If a prefix is not given on the command line,
 *  then the users insist that they be prompted for the prefix,
 *  to prevent inadvertently sucking in a non-prefixed file.
 *  Slash ("/") specifies no prefix.
 */
int
cmd_concat(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    /* get any prefix */
    if (argc < 2) {
	Tcl_AppendResult(interp, MORE_ARGS_STR,
			 "dbconcat: Enter database: ",
			 (char *)NULL);
	return TCL_ERROR;
    }
    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'p') {
	Tcl_AppendResult(interp, MORE_ARGS_STR,
			 "dbconcat: Enter database: ",
			 (char *)NULL);
	return TCL_ERROR;
    }
    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 's') {
	Tcl_AppendResult(interp, MORE_ARGS_STR,
			 "dbconcat: Enter database: ",
			 (char *)NULL);
	return TCL_ERROR;
    }
    if (argc < 3) {
	Tcl_AppendResult(interp, MORE_ARGS_STR,
			 "dbconcat: Enter prefix string or / for no prefix: ",
			 (char *)NULL);
	return TCL_ERROR;
    }
    if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 'p') {
	Tcl_AppendResult(interp, MORE_ARGS_STR,
			 "dbconcat: Enter prefix string or / for no prefix: ",
			 (char *)NULL);
	return TCL_ERROR;
    }
    if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 's') {
	Tcl_AppendResult(interp, MORE_ARGS_STR,
			 "dbconcat: Enter suffix string or / for no suffix: ",
			 (char *)NULL);
	return TCL_ERROR;
    }

    return wdb_concat_cmd(wdbp, interp, argc, argv);
}

/**
 * Rename an object
 * Format: mv oldname newname
 */
int
cmd_name(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_move_cmd(wdbp, interp, argc, argv);
}

/**
 * add solids to a region or create the region
 * and then add solids
 * Format: r regionname opr1 sol1 opr2 sol2 ... oprn soln
 */
int
cmd_region(ClientData	clientData,
	   Tcl_Interp	*interp,
	   int		argc,
	   char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_region_cmd(wdbp, interp, argc, argv);
}

/**
 * Delete members of a combination
 * Format: rm comb memb1 memb2 .... membn
 */
int
cmd_remove(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_remove_cmd(wdbp, interp, argc, argv);
}

/**
 *			C M D _ U N I T S
 *
 * Change the local units of the description.
 * Base unit is fixed in mm, so this just changes the current local unit
 * that the user works in.
 */
int
cmd_units(ClientData	clientData,
	  Tcl_Interp	*interp,
	  int		argc,
	  char		**argv)
{
    register struct dm_list *dmlp;
    int		ret;
    fastf_t		sf;

    CHECK_DBI_NULL;

    sf = dbip->dbi_base2local;
    ret = wdb_units_cmd(wdbp, interp, argc, argv);

    set_localunit_TclVar();
    sf = dbip->dbi_base2local / sf;
    update_grids(sf);
    update_views = 1;

    FOR_ALL_DISPLAYS(dmlp, &head_dm_list.l) {
	dmlp->dml_view_state->vs_vop->vo_local2base = dbip->dbi_local2base;
	dmlp->dml_view_state->vs_vop->vo_base2local = dbip->dbi_base2local;
    }

    return ret;
}

/**
 *	Change the current title of the description
 */
int
cmd_title(ClientData	clientData,
	  Tcl_Interp	*interp,
	  int		argc,
	  char		**argv)
{
    int	ret;

    CHECK_DBI_NULL;

    ret = wdb_title_cmd(wdbp, interp, argc, argv);
    view_state->vs_flag = 1;

    return ret;
}

/**
 *  			C M D _ P R C O L O R
 */
int
cmd_prcolor(ClientData	clientData,
	    Tcl_Interp	*interp,
	    int		argc,
	    char	**argv)
{
    return wdb_prcolor_cmd(wdbp, interp, argc, argv);
}

/** List object information, briefly
 * Format: cat object
 */
int
cmd_cat(ClientData	clientData,
	Tcl_Interp	*interp,
	int		argc,
	char		**argv)
{
    int ret;

    CHECK_DBI_NULL;

#if 0
    if (setjmp(jmp_env) == 0)
	(void)signal(SIGINT, sig3);	/* allow interupts */
    else
	return TCL_OK;
#endif

    ret = wdb_cat_cmd(wdbp, interp, argc, argv);

#if 0
    (void)signal(SIGINT, SIG_IGN);
#endif
    return ret;
}

/**
 *  			C M D _ C O L O R
 *
 *  Add a color table entry.
 */
int
cmd_color(ClientData	clientData,
	  Tcl_Interp	*interp,
	  int		argc,
	  char		**argv)
{
    int ret;

    CHECK_DBI_NULL;

    ret = wdb_color_cmd(wdbp, interp, argc, argv);
    color_soltab();

    return ret;
}

/**
 *			C M D _ C O M B
 *
 *  Create or add to the end of a combination, with one or more solids,
 *  with explicitly specified operations.
 *
 *  Format: comb comb_name sol1 opr2 sol2 ... oprN solN
 */
int
cmd_comb(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_comb_cmd(wdbp, interp, argc, argv);
}

/** Copy an object
 * Format: cp oldname newname
 */
int
cmd_copy(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    int ret;
    const char *av[3];

    CHECK_DBI_NULL;

    if ((ret = wdb_copy_cmd(wdbp, interp, argc, argv)) != TCL_OK)
	return ret;

    av[0] = "e";
    av[1] = argv[2]; /* depends on solid name being in argv[2] */
    av[2] = NULL;

    /* draw the new object */
    return cmd_draw(clientData, interp, 2, av);
}

/**
 *                C M D _ E X P A N D
 *
 * Performs wildcard expansion (matched to the database elements)
 * on its given arguments.  The result is returned by Tcl_GetStringResult(interp).
 */
int
cmd_expand(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    CHECK_DBI_NULL;

    return wdb_expand_cmd(wdbp, interp, argc, argv);
}

/**
 *			C M D _ L S
 *
 * This routine lists the names of all the objects accessible
 * in the object file.
 */
int
cmd_ls(ClientData	clientData,
       Tcl_Interp	*interp,
       int		argc,
       char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_ls_cmd(wdbp, interp, argc, argv);
}

/**
 *  			C M D _ F I N D
 *
 *  Find all references to the named objects.
 */
int
cmd_find(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_find_cmd(wdbp, interp, argc, argv);
}

/** Grouping command
 * Format: g groupname object1 object2 .... objectn
 */
int
cmd_group(ClientData	clientData,
	  Tcl_Interp	*interp,
	  int		argc,
	  char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_group_cmd(wdbp, interp, argc, argv);
}

/** Create an instance of something
 * Format: i object combname [op]
 */
int
cmd_instance(ClientData	clientData,
	     Tcl_Interp *interp,
	     int	argc,
	     char	**argv)
{
    CHECK_DBI_NULL;

    return wdb_instance_cmd(wdbp, interp, argc, argv);
}

int
cmd_keep(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    CHECK_DBI_NULL;

    return wdb_keep_cmd(wdbp, interp, argc, argv);
}

/**
 *			C M D _ L I S T
 *
 *  List object information, verbose, in GIFT-compatible format.
 *  Format: l object
 */
int
cmd_list(ClientData	clientData,
	 Tcl_Interp	*interp,
	 int		argc,
	 char		**argv)
{
    int recurse=0;

    CHECK_DBI_NULL;

    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'r' && argv[1][2] == '\0')
	recurse = 1;

    /*
     * Here we have no usable arguments,
     * so we better be in an edit state.
     */
    if ((argc == 1 || (argc == 2 && recurse)) && illump != SOLID_NULL) {
	struct bu_vls	vls;
	int		ret;
	int		ac;
	char		*av[4];

	bu_vls_init(&vls);

	if (state == ST_S_EDIT)
	    db_path_to_vls( &vls, &illump->s_fullpath );
	else if (state == ST_O_EDIT) {
	    register int	i;
	    for ( i=0; i < ipathpos; i++ ) {
		bu_vls_printf(&vls, "/%s",
			      DB_FULL_PATH_GET(&illump->s_fullpath, i)->d_namep );
	    }
	} else
	    return TCL_ERROR;

	if (recurse) {
	    av[0] = "l";
	    av[1] = "-r";
	    av[2] = bu_vls_addr(&vls);
	    av[3] = (char *)NULL;
	    ac = 3;
	} else {
	    av[0] = "l";
	    av[1] = bu_vls_addr(&vls);
	    av[2] = (char *)NULL;
	    ac = 2;
	}
	ret = wdb_list_cmd(wdbp, interp, ac, av);
	bu_vls_free(&vls);
	return ret;
    } else {
	return wdb_list_cmd(wdbp, interp, argc, argv);
    }
}

/**
 *			C M D _ L M
 *
 *	List regions based on values of their MUVES_Component attribute
 */
int
cmd_lm(ClientData	clientData,
       Tcl_Interp	*interp,
       int		argc,
       char		**argv)
{
    struct bu_attribute_value_set avs;
    struct bu_vls vls;
    struct bu_ptbl *tbl;
    struct directory *dp;
    int i;
    int last_opt=0;
    int new_arg_count=1;
    int new_argc;
    int ret;
    char **new_argv;

    bu_vls_init( &vls );
    bu_vls_strcat( &vls, argv[0] );
    for ( i=1; i<argc; i++ ) {
	if ( *argv[i] == '-' ) {
	    bu_vls_putc( &vls, ' ' );
	    bu_vls_strcat( &vls, argv[i] );
	    last_opt = i;
	    new_arg_count++;
	} else {
	    break;
	}
    }

    bu_avs_init( &avs, argc - last_opt, "cmd_lm avs" );
    for ( i=last_opt+1; i<argc; i++ ) {
	bu_avs_add_nonunique( &avs, "MUVES_Component", argv[i] );
    }

    tbl = db_lookup_by_attr( dbip, DIR_REGION, &avs, 2 );
    if ( !tbl ) {
	/* Error!!! */
	Tcl_AppendResult( interp, "Error: db_lookup_by_attr() failed!!\n", (char *)NULL );
	bu_vls_free( &vls );
	bu_avs_free( &avs );
	return TCL_ERROR;
    }

    if ( BU_PTBL_LEN( tbl ) == 0 ) {
	/* no matches */
	bu_vls_free( &vls );
	bu_avs_free( &avs );
	bu_ptbl_free( tbl );
	bu_free( (char *)tbl, "cmd_lm ptbl" );
	return TCL_OK;
    }

    for ( i=0; i<BU_PTBL_LEN( tbl ); i++ ) {
	dp = (struct directory *)BU_PTBL_GET( tbl, i );
	bu_vls_putc( &vls, ' ' );
	bu_vls_strcat( &vls, dp->d_namep );
	new_arg_count++;
    }

    bu_ptbl_free( tbl );
    bu_free( (char *)tbl, "cmd_lm ptbl" );

    /* create a new argc and argv to pass to the cmd_ls routine */
    new_argv = (char **)bu_calloc( new_arg_count+1, sizeof( char *), "cmd_lm new_argv" );
    new_argc = bu_argv_from_string( new_argv, new_arg_count, bu_vls_addr( &vls ) );

    ret = cmd_ls( clientData, interp, new_argc, new_argv );

    bu_vls_free( &vls );
    bu_free( (char *)new_argv, "cmd_lm new_argv" );

    return( ret );
}


/**
 *			C M D _ L T
 *
 *  List object information in a tcl list. The
 *  tcl list is a list of {op obj} pairs.
 */
int
cmd_lt(ClientData	clientData,
       Tcl_Interp	*interp,
       int		argc,
       char		**argv)
{
    return wdb_lt_cmd(wdbp, interp, argc, argv);
}

/**
 *			F _ T O L
 *
 *  "tol"	displays current settings
 *  "tol abs #"	sets absolute tolerance.  # > 0.0
 *  "tol rel #"	sets relative tolerance.  0.0 < # < 1.0
 *  "tol norm #" sets normal tolerance, in degrees.
 *  "tol dist #" sets calculational distance tolerance
 *  "tol perp #" sets calculational normal tolerance.
 */
int
cmd_tol(ClientData	clientData,
	Tcl_Interp	*interp,
	int		argc,
	char		**argv)
{
    int ret;

    CHECK_DBI_NULL;

    ret = wdb_tol_cmd(wdbp, interp, argc, argv);

    /* hack to keep mged tolerance settings current */
    mged_ttol = wdbp->wdb_ttol;
    mged_tol = wdbp->wdb_tol;
    mged_abs_tol = mged_ttol.abs;
    mged_rel_tol = mged_ttol.rel;
    mged_nrm_tol = mged_ttol.norm;

    return( ret );
}

/* defined in chgview.c */
extern int edit_com(int argc, const char *argv[], int kind, int catch_sigint);

/**
 * ZAP the display -- then edit anew
 * Format: B object
 */
int
cmd_blast(ClientData clientData, Tcl_Interp *interp, int argc, const char *argv[])
{
    const char *av[2];

    av[0] = "Z";
    av[1] = (char *)0;

    if (cmd_zap(clientData, interp, 1, av) == TCL_ERROR)
	return TCL_ERROR;
        
    if ( argc == 1 ) /* "B" alone is same as "Z" */
	return TCL_OK;

    return edit_com(argc, argv, 1, 1);
}

/**
 * Edit something (add to visible display)
 * Format: e object
 */
int
cmd_draw(ClientData clientData, Tcl_Interp *interp, int argc, const char *argv[])
{
    return edit_com(argc, argv, 1, 1);
}

extern int emuves_com( int argc, char **argv );	/* from chgview.c */

/**
 * Add regions with attribute MUVES_Component haveing the specified values
 * Format: em value [value value ...]
 */
int
cmd_emuves(ClientData	clientData,
	   Tcl_Interp	*interp,
	   int		argc,
	   char		**argv)
{
    return emuves_com(argc, argv);
}

/**
 * Format: ev objects
 */
int
cmd_ev(ClientData	clientData,
       Tcl_Interp *interp,
       int	argc,
       const char *argv[])
{
    return edit_com(argc, argv, 3, 1);
}

/**
 *			C M D _ V D R A W
 */
int
cmd_vdraw(ClientData	clientData,
	  Tcl_Interp	*interp,
	  int		argc,
	  char		**argv)
{
    CHECK_DBI_NULL;

    return vdraw_cmd(dgop, interp, argc, argv);
}

/**
 *			C M D _ E
 *
 *  The "Big E" command.
 *  Evaluated Edit something (add to visible display)
 *  Usage: E object(s)
 */
int
cmd_E(ClientData	clientData,
      Tcl_Interp	*interp,
      int		argc,
      char	**argv)
{
    int	initial_blank_screen;
    register struct dm_list *dmlp;
    register struct dm_list *save_dmlp;
    register struct cmd_list *save_cmd_list;
    int ret;

    CHECK_DBI_NULL;
    initial_blank_screen = BU_LIST_IS_EMPTY(&dgop->dgo_headSolid);

    if ((ret = dgo_E_cmd(dgop, interp, argc, argv)) != TCL_OK)
	return ret;

    update_views = 1;

    save_dmlp = curr_dm_list;
    save_cmd_list = curr_cmd_list;
    FOR_ALL_DISPLAYS(dmlp, &head_dm_list.l) {
	curr_dm_list = dmlp;
	if (curr_dm_list->dml_tie)
	    curr_cmd_list = curr_dm_list->dml_tie;
	else
	    curr_cmd_list = &head_cmd_list;

	/* If we went from blank screen to non-blank, resize */
	if (mged_variables->mv_autosize  && initial_blank_screen &&
	    BU_LIST_NON_EMPTY(&dgop->dgo_headSolid)) {
	    struct view_ring *vrp;

	    size_reset();
	    new_mats();
	    (void)mged_svbase();

	    for (BU_LIST_FOR(vrp, view_ring, &view_state->vs_headView.l))
		vrp->vr_scale = view_state->vs_vop->vo_scale;
	}
    }

    curr_dm_list = save_dmlp;
    curr_cmd_list = save_cmd_list;

    return TCL_OK;
}

int
cmd_bot_face_sort( ClientData	clientData,
		   Tcl_Interp	*interp,
		   int     	argc,
		   char    	**argv)
{
    CHECK_DBI_NULL;
    return wdb_bot_face_sort_cmd( wdbp, interp, argc, argv );
}

int
cmd_bot_decimate( ClientData	clientData,
		  Tcl_Interp	*interp,
		  int     	argc,
		  char    	**argv)
{
    CHECK_DBI_NULL;
    return wdb_bot_decimate_cmd( wdbp, interp, argc, argv );
}


int
cmd_shaded_mode(ClientData	clientData,
		Tcl_Interp	*interp,
		int     	argc,
		char    	**argv)
{
    /* check to see if we have -a or -auto */
    if (argc == 3 &&
	strlen(argv[1]) >= 2 &&
	argv[1][0] == '-' &&
	argv[1][1] == 'a') {
	struct bu_vls vls;

	/* set zbuffer, zclip and lighting for all */
	bu_vls_init(&vls);
	bu_vls_printf(&vls, "mged_shaded_mode_helper %s", argv[2]);
	Tcl_Eval(interp, bu_vls_addr(&vls));
	bu_vls_free(&vls);

	/* skip past -a */
	--argc;
	++argv;
    }

    return dgo_shaded_mode_cmd(dgop, interp, argc, argv);
}

/* XXX needs to be provided from points header */
extern int parse_point_file(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);

int
cmd_parse_points(ClientData	clientData,
		 Tcl_Interp	*interp,
		 int     	argc,
		 char    	**argv)
{

#ifndef BC_WITH_PARSERS
    bu_log("parse_points was disabled in this compilation of mged due to system limitations\n");
    return TCL_ERROR;
#else
    if (argc != 2) {
	bu_log("parse_points only supports a single file name right now\n");
	bu_log("doing nothing\n");
	return TCL_ERROR;
    }
    return parse_point_file(clientData, interp, argc-1, &(argv[1]));
#endif
}

int
cmd_has_embedded_fb(ClientData	clientData,
		    Tcl_Interp	*interp,
		    int     	argc,
		    char    	**argv)
{
    Tcl_DString ds;

    Tcl_DStringInit(&ds);
#ifdef USE_FBSERV
    Tcl_DStringAppend(&ds, "1", -1);
#else
    Tcl_DStringAppend(&ds, "0", -1);
#endif
    Tcl_DStringResult(interp, &ds);

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
