/*
 * zlibtclStubLib.c --
 *
 *	Stub object that will be statically linked into extensions that wish
 *	to access the ZLIBTCL API.
 *
 * Copyright (c) 2002 Andreas Kupries <andreas_kupries@users.sourceforge.net>
 * Copyright (c) 2002 Andreas Kupries <andreas_kupries@users.sourceforge.net>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: zlibtclStubLib.c 28910 2007-09-14 15:22:11Z erikgreenwald $
 */

#ifndef USE_TCL_STUBS
#define USE_TCL_STUBS
#endif

#include "zlibtcl.h"

ZlibtclStubs *zlibtclStubsPtr;

/*
 *----------------------------------------------------------------------
 *
 * Zlibtcl_InitStubs --
 *
 *	Checks that the correct version of Blt is loaded and that it
 *	supports stubs. It then initialises the stub table pointers.
 *
 * Results:
 *	The actual version of BLT that satisfies the request, or
 *	NULL to indicate that an error occurred.
 *
 * Side effects:
 *	Sets the stub table pointers.
 *
 *----------------------------------------------------------------------
 */

#ifdef Z_InitStubs
#undef Z_InitStubs
#endif

CONST char *
Z_InitStubs(interp, version, exact)
    Tcl_Interp *interp;
    CONST char *version;
    int exact;
{
    CONST char *result;

    /* HACK: de-CONST 'version' if compiled against 8.3.
     * The API has no CONST despite not modifying the argument
     * And a debug build with high warning-level on windows
     * will abort the compilation.
     */

#if ((TCL_MAJOR_VERSION < 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION < 4)))
#define UNCONST (char*)
#else
#define UNCONST
#endif

    result = Tcl_PkgRequireEx(interp, ZLIBTCL_PACKAGE_NAME, UNCONST version, exact,
		(ClientData *) &zlibtclStubsPtr);
    if (!result || !zlibtclStubsPtr) {
        return (char *) NULL;
    }

    return result;
}
#undef UNCONST
