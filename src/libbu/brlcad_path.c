/*                   B R L C A D _ P A T H . C
 * BRL-CAD
 *
 * Copyright (c) 2004-2008 United States Government as represented by
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
/** @addtogroup filesystem */
/** @{ */
/** @file brlcad_path.c
 *
 * @brief
 *  A support routine to provide the executable code with the path
 *  to where the BRL-CAD programs and libraries are installed.
 *
 */

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_SYS_PARAM_H
#  include <sys/param.h>
#endif

#include "bu.h"
#include "sysv.h"

/* private headers */
#include "brlcad_version.h"


/* defaults for configure-less compiles */
#ifndef BRLCAD_ROOT
#  define BRLCAD_ROOT "/usr/brlcad"
#endif


/* arbitrary buffer size large enough to hold a couple paths and a label */
#define MAX_WHERE_SIZE (size_t)((MAXPATHLEN*2) + 64)


/* internal storage for bu_getprogname/bu_setprogname */
static char bu_progname[MAXPATHLEN] = {0};


static const char *
brlcad_data()
{
#ifndef BRLCAD_DATA
    static char path[MAXPATHLEN] = {0};
    snprintf(path, MAXPATHLEN, "%s/share/brlcad/%s", BRLCAD_ROOT, brlcad_version());
    return path;
#else
    return BRLCAD_DATA;
#endif
}


/**
 * b u _ i p w d
 *
 * set/return the path to the initial working directory.
 * bu_setprogname() must be called on app startup for the correct pwd to
 * be acquired/set.
 */
static const char *
bu_ipwd()
{
    /* private stash */
    static const char *ipwd = NULL;
    static char buffer[MAXPATHLEN] = {0};

    if (ipwd) {
	return ipwd;
    }

    ipwd = getenv("PWD"); /* not our memory to free */

    if (!ipwd && (ipwd = bu_which("pwd"))) {
	FILE *fp;

	fp = popen(ipwd, "r");
	if (fp) {
	    if (bu_fgets(buffer, MAXPATHLEN, fp)) {
		ipwd = buffer;
	    } else {
		ipwd = ".";
	    }
	} else {
	    ipwd = ".";
	}
    } else {
	ipwd = ".";
    }

    return ipwd;
}


/**
 * b u _ a r g v 0
 *
 * this routine is used by the brlcad-path-finding routines when
 * attempting to locate binaries, libraries, and resources.  This
 * routine will set argv0 if path is provided and should generally be
 * set early on by bu_setprogname().
 */
const char *
bu_argv0(void)
{
    /* private stash */
    static const char *argv0 = NULL;

    /* set initial pwd if we have not already */
    (void)bu_ipwd();

    if (bu_progname[0] != '\0') {
	argv0 = bu_progname;
    }

#ifdef HAVE_GETPROGNAME
    if (!argv0) {
	/* do not call bu_getgrogname() */
	argv0 = getprogname(); /* not malloc'd memory */
    }
#endif

    return argv0;
}


/**
 * b u _ a r g v 0 _ f u l l _ p a t h
 *
 * returns the full path to argv0, regardless of how the application
 * was invoked.
 */
const char *
bu_argv0_full_path(void)
{
    static char buffer[MAXPATHLEN] = {0};

    const char *argv0 = bu_argv0();
    const char *ipwd = bu_ipwd();

    const char *which = bu_which(argv0);

    if (argv0[0] == BU_DIR_SEPARATOR) {
	/* seems to be a full path */
	snprintf(buffer, MAXPATHLEN, "%s", argv0);
	return buffer;
    }

    if (argv0[0] == '.' && argv0[1] == BU_DIR_SEPARATOR) {
	/* remove a ./ if present */
	argv0 += 2;
    }

    /* running from installed */
    if (which) {
	snprintf(buffer, MAXPATHLEN, "%s", which);
	return buffer;
    }

    /* running from source dir */
    snprintf(buffer, MAXPATHLEN, "%s%c%s", ipwd, BU_DIR_SEPARATOR, argv0);
    if (bu_file_exists(buffer)) {
	return buffer;
    }

    /* give up */
    snprintf(buffer, MAXPATHLEN, "%s", argv0);
    return buffer;
}


/** b u _ g e t p r o g n a m e
 *
 * get the name of the running application if they ran
 * bu_setprogname() first or if we know what it's supposed to be
 * anyways.
 */
const char *
bu_getprogname(void) {
    const char *name = NULL;

    if (bu_progname[0] != '\0') {
	return bu_basename(bu_progname);
    }

#ifdef HAVE_GETPROGNAME
    name = getprogname(); /* not malloc'd memory */
#endif

    if (!name) {
	name = bu_argv0();
    }

    snprintf(bu_progname, MAXPATHLEN, "%s", name?name:"unknown");

    return bu_basename(bu_progname);
}


/** b u _ s e t p r o g n a m e
 *
 * Set the name of the running application.  This isn't necessary on
 * modern systems that support getprogname() and call setprogname()
 * before main() for you, but necessary otherwise for portability.
 */
void
bu_setprogname(const char *argv0)
{
#ifdef HAVE_SETPROGNAME
    setprogname(argv0);
#endif

    if (argv0) {
	snprintf(bu_progname, MAXPATHLEN, "%s", argv0);
    }

    (void)bu_ipwd();

    return;
}


/** b u _ r o o t _ m i s s i n g
 *
 *print out an error/warning message if we cannot find the specified
 * BRLCAD_ROOT (compile-time install path)
 */
static void
bu_root_missing(const char *paths)
{
    bu_log("\
Unable to locate where BRL-CAD %s is installed while searching:\n\
%s\n\
This version of BRL-CAD was compiled to be installed at:\n\
	%s\n\n", brlcad_version(), paths, BRLCAD_ROOT);

#ifndef _WIN32
    bu_log("\
You may specify where to locate BRL-CAD by setting the BRLCAD_ROOT\n\
environment variable.  For example:\n\
\n\
for csh/tcsh users:\n\
	setenv BRLCAD_ROOT /path/to/brlcad\n\
for sh/bash users:\n\
	BRLCAD_ROOT=/path/to/brlcad ; export BRLCAD_ROOT\n\n");
#endif

    return;
}


/**
 * b u _ d a t a _ m i s s i n g
 *
 * print out an error/warning message if we cannot find the specified
 * BRLCAD_DATA (compile-time install path)
 */
static void
bu_data_missing(const char *paths)
{
    bu_log("\
Unable to locate where BRL-CAD %s data resources are installed\n\
while searching:\n\
%s\n\
This release of BRL-CAD expects data resources to be at:\n\
	%s\n\n", brlcad_version(), paths, brlcad_data());

#ifndef _WIN32
    bu_log("\
You may specify where to locate BRL-CAD data resources by setting\n\
the BRLCAD_DATA environment variable.  For example:\n\
\n\
for csh/tcsh users:\n\
	setenv BRLCAD_DATA /path/to/brlcad/data\n\
for sh/bash users:\n\
	BRLCAD_DATA=/path/to/brlcad/data ; export BRLCAD_DATA\n\n");
#endif

    return;
}


/**
 * put a left-hand and right-hand path together and test whether they
 * exist or not.
 *
 * @return boolean on whether a match was found.
 */
static int
bu_find_path(char result[MAXPATHLEN], const char *lhs, const char *rhs, struct bu_vls *searched, const char *where)
{
    int llen, rlen;
    static const char *currdir=".";

    /* swap right with left if there is no left so logic is simplified
     * later on.
     */
    if (lhs == NULL && rhs != NULL) {
	lhs = rhs;
	rhs = NULL;
    }

    if (lhs == NULL) {
	if (searched && where) {
	    bu_vls_strcat(searched, where);
	}
	return 0;
    }

    rlen = llen = 0;
    if (rhs) {
	rlen = strlen(rhs);
    }

    /* be safe */
    if (rlen + 2 > MAXPATHLEN) {
	bu_log("Warning: path is way too long (%d characters > %d)\n", rlen+2, MAXPATHLEN);
	if (searched && where) {
	    bu_vls_strcat(searched, where);
	}
	return 0;
    }

    /* an empty left hand implies current directory (plus a slash is appended later) */
    if (lhs[0] == '\0') {
	lhs = currdir;
    }

    /* left-hand path should exist independent of right-hand path */
    if (!bu_file_exists(lhs)) {
	if (searched && where) {
	    bu_vls_strcat(searched, where);
	}
	return 0;
    }

    /* start fresh */
    memset(result, 0, (size_t)MAXPATHLEN);
    bu_strlcpy(result, lhs, (size_t)MAXPATHLEN);

    /* nothing to add, so just return what we have */
    if (!rhs || (rlen == 0)) {
	return 1;
    }

    /* be safe again */
    llen = strlen(result);
    if (llen + rlen + 3 > MAXPATHLEN) {
	bu_log("Warning: path is way too long (%d characters > %d)\n", llen+rlen+3, MAXPATHLEN);
	if (searched && where) {
	    bu_vls_strcat(searched, where);
	}
	return 0;
    }

    if ( (*(result+llen-1) != BU_DIR_SEPARATOR) && (rhs[0] != BU_DIR_SEPARATOR) ) {
	/* let the caller give "/usr/brlcad" and "bin" and get "/usr/brlcad/bin" */
	*(result+llen) = BU_DIR_SEPARATOR;
	llen++;
    } else if ( (*(result+llen-1) == BU_DIR_SEPARATOR) && (rhs[0] == BU_DIR_SEPARATOR) ) {
	/* let the caller give "/usr/brlcad/" and "/bin" and get "/usr/brlcad/bin"*/
	rhs++;
	rlen--;
    }

    /* found a match */
    bu_strlcpy(result+llen, rhs, (size_t)(MAXPATHLEN - llen));
    if (bu_file_exists(result)) {
	return 1;
    }

    /* close, but no match */
    if (searched && where) {
	bu_vls_strcat(searched, where);
    }
    return 0;
}


/**
 * b u _ b r l c a d _ r o o t
 *
 * Locate where the BRL-CAD applications and libraries are installed.
 *
 * The BRL-CAD root is searched for in the following order of
 * precedence by testing for the rhs existence if provided or the
 * directory existence otherwise:
 *
 *   BRLCAD_ROOT environment variable if set
 *   BRLCAD_ROOT compile-time path
 *   run-time path identification
 *   /usr/brlcad static path
 *   current directory
 *
 * @return
 * A STATIC buffer is returned.
 * It is the caller's responsibility to call bu_strdup() or make
 * other provisions to save the returned string, before calling again.
 */
const char *
bu_brlcad_root(const char *rhs, int fail_quietly)
{
    static char result[MAXPATHLEN] = {0};
    const char *lhs;
    struct bu_vls searched;
    char where[MAX_WHERE_SIZE] = {0};

    bu_vls_init(&searched);

    if (bu_debug & BU_DEBUG_PATHS) {
	bu_log("bu_brlcad_root: searching for [%s]\n", rhs?rhs:"");
    }

    /* BRLCAD_ROOT environment variable if set */
    lhs = getenv("BRLCAD_ROOT");
    if (lhs) {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_ROOT environment variable [%s]\n", lhs);
	if (bu_find_path(result, lhs, rhs, &searched, where)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: BRLCAD_ROOT environment variable [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    } else {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_ROOT environment variable\n");
	bu_vls_strcat(&searched, where);
    }

    /* BRLCAD_ROOT compile-time path */
    lhs = BRLCAD_ROOT;
    if (lhs) {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_ROOT compile-time path [%s]\n", lhs);
	if (bu_find_path(result, lhs, rhs, &searched, where)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: BRLCAD_ROOT compile-time path [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    } else {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_ROOT compile-time path [UNKNOWN]\n");
	bu_vls_strcat(&searched, where);
    }

    /* run-time path identification */
    lhs = bu_getprogname();
    if (lhs) {
	char argv0[MAX_WHERE_SIZE] = {0};
	int len = strlen(lhs);
	snprintf(argv0, MAX_WHERE_SIZE, "%s", lhs);

	/* need to trim off the trailing binary */
	while (len-1 > 0) {
	    if (argv0[len-1] == BU_DIR_SEPARATOR) {
		argv0[len] = '.';
		argv0[len+1] = '.';
		argv0[len+2] = '\0';
		break;
	    }
	    len--;
	}

	snprintf(where, MAX_WHERE_SIZE, "\trun-time path identification [%s]\n", argv0);
	if (bu_find_path(result, argv0, rhs, &searched, where)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: Run-time path identification [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    } else {
	snprintf(where, MAX_WHERE_SIZE, "\trun-time path identification [UNKNOWN]\n");
	bu_vls_strcat(&searched, where);
    }

    /* /usr/brlcad static path */
    {
	const char *root = BRLCAD_ROOT;
	/* only check /usr/brlcad if not already tested earlier via BRLCAD_ROOT */

	if (root[0] != '/' || root[1] != 'u' || root[ 2] != 's' || root[ 3] != 'r' ||
	    root[4] != '/' || root[5] != 'b' || root[ 6] != 'r' || root[ 7] != 'l' ||
	    root[8] != 'c' || root[9] != 'a' || root[10] != 'd' || root[11] != '\0') {

	    if (bu_find_path(result, "/usr/brlcad", rhs, &searched, "\t/usr/brlcad default path\n")) {
		if (bu_debug & BU_DEBUG_PATHS) {
		    bu_log("Found: /usr/brlcad default path [%s]\n", result);
		}
		bu_vls_free(&searched);
		return result;
	    }
	}
    }

    /* current directory */
    if (bu_find_path(result, ".", rhs, &searched, "\tcurrent directory\n")) {
	if (bu_debug & BU_DEBUG_PATHS) {
	    bu_log("Found: current directory [%s]\n", result);
	}
	bu_vls_free(&searched);
	return result;
    }

    if (!fail_quietly) {
	bu_root_missing(bu_vls_addr(&searched));
	if (rhs) {
	    bu_log("Unable to find '%s' within the BRL-CAD software installation.\nThis copy of BRL-CAD may not be properly installed.\n\n", rhs);
	} else {
	    bu_log("Unable to find the BRL-CAD software installation.\nThis copy of BRL-CAD may not be properly installed.\n\n");
	}
    }

    bu_vls_free(&searched);
    return NULL;
}


/**
 * b u _ b r l c a d _ d a t a
 *
 * Locate where the BRL-CAD data resources are installed.
 *
 * The BRL-CAD data resources are searched for in the following order
 * of precedence by testing for the existence of rhs if provided or
 * the directory existence otherwise:
 *
 *   BRLCAD_DATA environment variable if set
 *   BRLCAD_DATA compile-time path
 *   bu_brlcad_root/share/brlcad/VERSION path
 *   bu_brlcad_root path
 *   current directory
 *
 * A STATIC buffer is returned.
 * It is the caller's responsibility to call bu_strdup() or make
 * other provisions to save the returned string, before calling again.
 */
const char *
bu_brlcad_data(const char *rhs, int fail_quietly)
{
    static char result[MAXPATHLEN] = {0};
    const char *lhs;
    struct bu_vls searched;
    char where[MAX_WHERE_SIZE] = {0};
    char path[MAXPATHLEN] = {0};

    bu_vls_init(&searched);

    if (bu_debug & BU_DEBUG_PATHS) {
	bu_log("bu_brlcad_data: looking for [%s]\n", rhs?rhs:"");
    }

    /* BRLCAD_DATA environment variable if set */
    lhs = getenv("BRLCAD_DATA");
    if (lhs) {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_DATA environment variable [%s]\n", lhs);
	if (bu_find_path(result, lhs, rhs, &searched, where)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: BRLCAD_DATA environment variable [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    } else {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_DATA environment variable\n");
	bu_vls_strcat(&searched, where);
    }

    /* BRLCAD_DATA compile-time path */
    lhs = brlcad_data();
    if (lhs) {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_DATA compile-time path [%s]\n", lhs);
	if (bu_find_path(result, lhs, rhs, &searched, where)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: BRLCAD_DATA compile-time path [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    } else {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_DATA compile-time path [UNKNOWN]\n");
	bu_vls_strcat(&searched, where);
    }

    /* bu_brlcad_root/share/brlcad/VERSION path */
    snprintf(path, (size_t)MAXPATHLEN, "share/brlcad/%s", brlcad_version());
    lhs = bu_brlcad_root(path, 1);
    if (lhs) {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_ROOT common data path  [%s]\n", path);
	if (bu_find_path(result, lhs, rhs, &searched, where)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: BRLCAD_ROOT common data path [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    }

    /* bu_brlcad_root/share/brlcad path */
    lhs = bu_brlcad_root("share/brlcad", 1);
    if (lhs) {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_ROOT common data path  [%s]\n", lhs);
	if (bu_find_path(result, lhs, rhs, &searched, where)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: BRLCAD_ROOT common data path [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    }

    /* bu_brlcad_root/share path */
    lhs = bu_brlcad_root("share", 1);
    if (lhs) {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_ROOT common data path  [%s]\n", lhs);
	if (bu_find_path(result, lhs, rhs, &searched, where)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: BRLCAD_ROOT common data path [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    }

    /* bu_brlcad_root path */
    lhs = bu_brlcad_root("", 1);
    if (lhs) {
	snprintf(where, MAX_WHERE_SIZE, "\tBRLCAD_ROOT common data path  [%s]\n", lhs);
	if (bu_find_path(result, lhs, rhs, &searched, where)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: BRLCAD_ROOT common data path [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    }

    /* current directory (running from uninstalled source distribution) */
    if (bu_find_path(result, ".", rhs, &searched, "\tcurrent directory\n")) {
	if (bu_debug & BU_DEBUG_PATHS) {
	    bu_log("Found: current directory [%s]\n", result);
	}
	bu_vls_free(&searched);
	return result;
    }

    /* running from uninstalled source distribution, look for THIS file */
#define BPC "/src/libbu/" __FILE__
    if (bu_file_exists("." BPC)) {
	if (bu_find_path(result, ".", rhs, NULL, NULL)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: source directory [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	} else if (bu_find_path(result, "./src", rhs, NULL, NULL)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: source directory [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    }
    if (bu_file_exists(".." BPC)) {
	if (bu_find_path(result, "..", rhs, NULL, NULL)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: source directory [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	} else if (bu_find_path(result, "../src", rhs, NULL, NULL)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: source directory [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    }
    if (bu_file_exists("../.." BPC)) {
	if (bu_find_path(result, "../..", rhs, NULL, NULL)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: source directory [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	} else if (bu_find_path(result, "../../src", rhs, NULL, NULL)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: source directory [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    }
    if (bu_file_exists("../../.." BPC)) {
	if (bu_find_path(result, "../../..", rhs, NULL, NULL)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: source directory [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	} else if (bu_find_path(result, "../../../src", rhs, NULL, NULL)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: source directory [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    }
    if (bu_file_exists("../../../.." BPC)) {
	if (bu_find_path(result, "../../../..", rhs, NULL, NULL)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: source directory [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	} else if (bu_find_path(result, "../../../../src", rhs, NULL, NULL)) {
	    if (bu_debug & BU_DEBUG_PATHS) {
		bu_log("Found: source directory [%s]\n", result);
	    }
	    bu_vls_free(&searched);
	    return result;
	}
    }

    if (!fail_quietly) {
	bu_data_missing(bu_vls_addr(&searched));
	if (rhs) {
	    bu_log("Unable to find '%s' within the BRL-CAD software installation.\nThis copy of BRL-CAD may not be properly installed.\n\n", rhs);
	} else {
	    bu_log("Unable to find the BRL-CAD software installation.\nThis copy of BRL-CAD may not be properly installed.\n\n");
	}
    }

    bu_vls_free(&searched);
    return NULL;
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
