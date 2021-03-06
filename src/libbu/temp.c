/*                           T E M P . C
 * BRL-CAD
 *
 * Copyright (c) 2001-2008 United States Government as represented by
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
/** @addtogroup bu_log */
/** @{ */
/** @file temp.c
 *
 * Routine to open a temporary file.
 *
 */

#include "common.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif
#include "bio.h"

#include "bu.h"

#define _TF_FAIL "WARNING: Unable to create a temporary file\n"


struct _bu_tf_list {
    struct bu_list l;
    struct bu_vls fn;
    int fd;
};

static int _bu_temp_files = 0;
static struct _bu_tf_list *_bu_tf = NULL;


static void
_bu_close_files()
{
    struct _bu_tf_list *popped;
    if (!_bu_tf) {
	return;
    }

    /* close all files, free their nodes, and unlink */
    while (BU_LIST_WHILE(popped, _bu_tf_list, &(_bu_tf->l))) {
	BU_LIST_DEQUEUE(&(popped->l));
	if (popped) {
	    if (popped->fd != -1) {
		close(popped->fd);
		popped->fd = -1;
	    }
	    if (BU_VLS_IS_INITIALIZED(&popped->fn) && bu_vls_addr(&popped->fn)) {
		unlink(bu_vls_addr(&popped->fn));
		bu_vls_free(&popped->fn);
	    }
	    bu_free(popped, "free bu_temp_file node");
	}
    }

    /* free the head */
    if (_bu_tf->fd != -1) {
	close(_bu_tf->fd);
	_bu_tf->fd = -1;
    }
    if (BU_VLS_IS_INITIALIZED(&_bu_tf->fn) && bu_vls_addr(&_bu_tf->fn)) {
	unlink(bu_vls_addr(&_bu_tf->fn));
	bu_vls_free(&_bu_tf->fn);
    }
    bu_free(_bu_tf, "free bu_temp_file head");
}


static void
_bu_add_to_list(const char *fn, int fd)
{
    struct _bu_tf_list *newtf;

    _bu_temp_files++;

    if (_bu_temp_files == 1) {
	/* schedule files for closure on exit */
	atexit(_bu_close_files);

	BU_GETSTRUCT(_bu_tf, _bu_tf_list);
	BU_LIST_INIT(&(_bu_tf->l));
	bu_vls_init(&_bu_tf->fn);

	bu_vls_strcpy(&_bu_tf->fn, fn);
	_bu_tf->fd = fd;
	
	return;
    }

    BU_GETSTRUCT(newtf, _bu_tf_list);
    bu_vls_init(&_bu_tf->fn);

    bu_vls_strcpy(&_bu_tf->fn, fn);
    newtf->fd = fd;

    BU_LIST_PUSH(&(_bu_tf->l), &(newtf->l));

    return;
}


#ifndef HAVE_MKSTEMP
static int
mkstemp(char *file_template)
{
    int fd = -1;
    int counter = 0;
    char *filepath = NULL;
    int i;
    int start, end;

    static const char replace[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static int replacelen = sizeof(replace) - 1;

    if (!file_template || file_template[0] == '\0')
	return -1;

    /* identify the replacement suffix */
    start = end = strlen(file_template)-1;
    for (i=strlen(file_template)-1; i>=0; i--) {
	if (file_template[i] != 'X') {
	    break;
	}
	end = i;
    }

    do {
	/* replace the template with random chars */
	srand((unsigned)time(NULL));
	for (i=start; i>=end; i--) {
	    file_template[i] = replace[(int)(replacelen * ((double)rand() / (double)RAND_MAX))];
	}
	fd = open(file_template, O_CREAT | O_EXCL | O_TRUNC | O_RDWR | O_TEMPORARY, S_IRUSR | S_IWUSR);
    } while ((fd == -1) && (counter++ < 1000));

    return fd;
}
#endif


/**
 *  b u _ t e m p _ f i l e
 *
 * Create a temporary file.  The first readable/writable directory
 * will be used, searching TMPDIR/TEMP/TMP environment variable
 * directories followed by default system temp directories and
 * ultimately trying the current directory.
 *
 * This routine is guaranteed to return a new unique file or return
 * NULL on failure.  The temporary file will be automatically unlinked
 * on application exit.  It is the caller's responsibility to set file
 * access settings, preserve file contents, or destroy file contents
 * if the default behavior is non-optimal.
 *
 * The name of the temporary file will be copied into a user-provided
 * (filepath) buffer if it is a non-NULL pointer and of a sufficient
 * (len) length to contain the filename.
 *
 * This routine is NOT thread-safe.
 *
 * Typical Use:
 @code
 *	FILE *fp;
 *	char filename[MAXPATHLEN];
 *	fp = bu_temp_file(&filename, MAXPATHLEN); // get file name
 *	...
 *	fclose(fp); // optional, auto-closed on exit
 *
 *	...
 *
 *	fp = bu_temp_file(NULL, 0); // don't need file name
 *      fchmod(fileno(fp), 0777);
 *	...
 *	rewind(fp);
 *	while (fputc(0, fp) == 0);
 *	fclose(fp);
 @endcode
*/
FILE *
bu_temp_file(char *filepath, size_t len)
{
    FILE *fp = NULL;
    int i;
    int fd = -1;
    char tempfile[MAXPATHLEN];
    const char *dir = NULL;
    const char *envdirs[] = {"TMPDIR", "TEMP", "TMP", NULL};
    const char *trydirs[] = {
#ifdef _WIN32
	"C:\\TEMP",
	"C:\\WINDOWS\\TEMP",
#endif
	"/tmp",
	"/usr/tmp",
	"/var/tmp", 
	".", /* last resort */
	NULL
    };

    if (len > MAXPATHLEN) {
	len = MAXPATHLEN;
    }

    /* check environment variable directories */
    for (i=0; envdirs[i]; i++) {
	dir = getenv(envdirs[i]);
	if (dir && dir[0] != '\0' && bu_file_writable(dir) && bu_file_executable(dir)) {
	    break;
	}
    }

    if (!dir) {
	/* try various directories */
	for (i=0; trydirs[i]; i++) {
	    dir = trydirs[i];
	    if (dir && dir[0] != '\0' && bu_file_writable(dir) && bu_file_executable(dir)) {
		break;
	    }
	}
    }

    if (!dir) {
	/* give up */
	bu_log("Unable to find a suitable temp directory\n");
	bu_log(_TF_FAIL);
	return NULL;
    }

    snprintf(tempfile, MAXPATHLEN, "%s%cBRL-CAD_temp_XXXXXXX", dir, BU_DIR_SEPARATOR);

    fd = mkstemp(tempfile);

    if (fd == -1) {
	perror("mkstemp");
	bu_log(_TF_FAIL);
	return NULL;
    }

    if (filepath) {
	if (len < strlen(tempfile)) {
#if 0
	    bu_log("WARNING: bu_temp_file filepath buffer size is insufficient (%d < %d)\n", len, strlen(tempfile));
#endif
	} else {
	    snprintf(filepath, len, "%s", tempfile);
	}
    }

    fp = fdopen(fd, "wb+");
    if (fp == NULL) {
	perror("fdopen");
	bu_log(_TF_FAIL);
	close(fd);
	return NULL;
    }

    /* add the file to the atexit auto-close list */
    _bu_add_to_list(tempfile, fd);

    return fp;
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
